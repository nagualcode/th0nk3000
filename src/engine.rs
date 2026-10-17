//! Granular engine: a faithful port of thOnk_0+2's rendering pipeline.
//!
//! The original never ran in real time. `BreakPointsToSoundFile` walks score time in
//! 10 ms steps and, for every grain due, calls `PutGrain` -> `GrainToDisk`, which
//! renders the *whole* grain (`GetGrain`, `ShapeGrain2`, constant-power pan) and
//! mixes it into the output file with a read-add-write of 16-bit samples
//! (`AIFFSndAddStereo`). That 16-bit add is where the famous wrap-around happens.
//!
//! This engine does the same thing into an accumulation ring buffer instead of a
//! file: grains are mixed whole, a bounded distance ahead of the playback position.
//! The ring holds i32 sums of i16 grain samples; since addition modulo 2^16 is
//! associative, casting that sum to i16 is bit-identical to the original's
//! wrap-on-every-add. Everything runs in the source file's sample rate, like the
//! original (whose output rate equals the input rate); only the final read is
//! resampled to the host rate.

use crate::aiff::{AudioBufferData, Pcm16Writer};
use crate::score::{trunc16, MacRandom, ScoreState, ScoreType};
use truce::prelude::ParamEnum;

/// Score-time step of `BreakPointsToSoundFile` (TOC+0x7e0).
const STEP_SECS: f64 = 0.01;
/// `GrainToDisk` clamps grain length to 441000 samples (0x6baa8).
const MAX_GRAIN_LEN: i64 = 441_000;
/// Ring size in samples per channel (2^20: ~23 s at 44.1 kHz, ~5 s at 192 kHz).
const RING_LEN: usize = 1 << 20;
const RING_MASK: i64 = RING_LEN as i64 - 1;
/// How far ahead of playback grains are mixed, in seconds. Grains that the original
/// would have placed up to this far "in the past" of the grain clock still land exactly.
const LOOKAHEAD_SECS: f64 = 1.0;
/// Length of the live-input capture buffer in seconds.
const LIVE_SECS: f64 = 5.0;
/// Upper bound of the density meter (the manual's "maximum vertical density").
pub const DENSITY_METER_MAX: usize = 600;

/// Cosine attack/decay tables built by `MakeCosineAttack` / `MakeCosineDecay`
/// (1000 entries each, `InitEnvelops(1000, 1000)`).
struct Envelopes {
    attack: [i32; 1000],
    decay: [i32; 1000],
}

impl Envelopes {
    fn new() -> Self {
        // The binary's own value of pi (TOC+0x7f0).
        #[allow(clippy::approx_constant)]
        const PI: f64 = 3.1415926536;
        let mut attack = [0; 1000];
        let mut decay = [0; 1000];
        for i in 0..1000 {
            let c = ((2 * i) as f64 / 2000.0 * PI).cos();
            attack[i] = (-(16384.0 * c - 16384.0)) as i32 as i16 as i32;
            decay[i] = (16383.0 * c + 16384.0) as i32 as i16 as i32;
        }
        Self { attack, decay }
    }
}

pub struct GranularEngine {
    // Source material, as 16-bit PCM like the original.
    pub source: AudioBufferData,
    live: Vec<i16>,
    live_write: usize,
    use_live: bool,
    host_sr: f64,

    // Composition.
    score: ScoreState,
    score_type: ScoreType,
    seed_rng: MacRandom,
    rng: MacRandom,
    pub last_seed: u32,

    // Scheduler state (`BreakPointsToSoundFile`).
    sched_t: f64,
    grain_t: f64,
    acc: f64,
    finished: bool,
    density: f64,
    /// Absolute ring position (domain samples) of composition time 0.
    base: i64,

    // Accumulation ring and playback.
    ring_l: Vec<i32>,
    ring_r: Vec<i32>,
    read_pos: f64,
    cleared_to: i64,
    max_end: i64,
    env: Envelopes,

    /// Fractional scheduler steps this block may take: work is spread evenly over
    /// blocks instead of bursting when the look-ahead refills.
    step_credit: f64,

    active: Vec<(i64, i64)>,
    pub active_grain_count: usize,
    pub auto_restart: bool,
}

impl GranularEngine {
    pub fn new(host_sr: f64) -> Self {
        let seed = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .map(|d| d.as_secs() as u32 ^ d.subsec_nanos())
            .unwrap_or(0x4170);
        let mut engine = Self {
            source: AudioBufferData::default_chime(44100.0),
            live: vec![0; (host_sr * LIVE_SECS) as usize],
            live_write: 0,
            use_live: false,
            host_sr,
            score: ScoreState::new(),
            score_type: ScoreType::ClassicFlowing,
            seed_rng: MacRandom::new(seed),
            rng: MacRandom::new(1),
            last_seed: 0,
            sched_t: 0.0,
            grain_t: 0.0,
            acc: 0.0,
            finished: true,
            density: 1.0,
            base: 0,
            ring_l: vec![0; RING_LEN],
            ring_r: vec![0; RING_LEN],
            read_pos: 0.0,
            cleared_to: -4,
            max_end: 0,
            env: Envelopes::new(),
            step_credit: 0.0,
            active: Vec::with_capacity(16384),
            active_grain_count: 0,
            auto_restart: true,
        };
        engine.trigger(ScoreType::ClassicFlowing, None);
        engine
    }

    /// Sample rate of the synthesis domain: the source file's rate, or the host's
    /// when granulating live input.
    fn domain_sr(&self) -> f64 {
        if self.use_live {
            self.host_sr
        } else {
            self.source.sample_rate
        }
    }

    fn source_duration(&self) -> f64 {
        if self.use_live {
            self.live.len() as f64 / self.host_sr
        } else {
            self.source.pcm16.len() as f64 / self.source.sample_rate
        }
    }

    /// Host rate change. Reallocates the live buffer, so call it off the audio path
    /// (`reset`), never from `process`.
    pub fn set_host_sample_rate(&mut self, sr: f64) {
        if sr != self.host_sr {
            self.host_sr = sr;
            self.live = vec![0; (sr * LIVE_SECS) as usize];
            self.live_write = 0;
        }
        let score_type = self.score_type;
        self.trigger(score_type, None);
    }

    /// Swaps in a new source and returns the previous one (so the caller can drop it
    /// off the audio thread). Starts a new composition.
    pub fn set_source(&mut self, data: AudioBufferData) -> AudioBufferData {
        let old = std::mem::replace(&mut self.source, data);
        let score_type = self.score_type;
        self.trigger(score_type, None);
        old
    }

    pub fn set_live(&mut self, live: bool) {
        if live != self.use_live {
            self.use_live = live;
            let score_type = self.score_type;
            self.trigger(score_type, None);
        }
    }

    /// Hit "Thonk": regenerate the score (seeded like `GetDateTime(&qd.randSeed)`,
    /// or with a fixed seed for recall) and restart from silence.
    pub fn trigger(&mut self, score_type: ScoreType, seed: Option<u32>) {
        let seed = seed.unwrap_or_else(|| self.seed_rng.next_seed());
        self.last_seed = seed;
        self.score_type = score_type;
        let dur = self.source_duration();
        self.score.regenerate(score_type, dur, seed);
        // The engine keeps drawing from the generator the score left off with.
        self.rng = MacRandom::new(seed ^ 0x5468_4f6e);
        // Only the part already mixed ahead of playback needs silencing; the rest
        // of the ring is zero (cleared behind the read head), so a new Thonk costs
        // ~1 s of memory traffic instead of the whole 8 MB ring.
        let from = self.read_pos as i64;
        for j in from..self.max_end.max(from) {
            let i = (j & RING_MASK) as usize;
            self.ring_l[i] = 0;
            self.ring_r[i] = 0;
        }
        self.active.clear();
        self.step_credit = 0.0;
        self.max_end = from;
        self.base = if from == 0 { 0 } else { from + 3 };
        self.start_composition();
    }

    fn start_composition(&mut self) {
        self.sched_t = 0.0;
        self.grain_t = 0.0;
        self.acc = 0.0;
        self.finished = false;
    }

    /// The score actually playing (resolves "ztochazt!k" to the chosen score).
    pub fn resolved_score(&self) -> ScoreType {
        self.score.resolved
    }

    pub fn score_progress(&self) -> f64 {
        (self.sched_t / self.score.total_duration).clamp(0.0, 1.0)
    }

    pub fn is_finished(&self) -> bool {
        self.finished && self.read_pos as i64 >= self.max_end
    }

    /// Pushes incoming live audio into the capture ring (mono, 16-bit).
    pub fn feed_live_input(&mut self, left: &[f32], right: &[f32]) {
        let len = self.live.len();
        if len == 0 {
            return;
        }
        for (i, &l) in left.iter().enumerate() {
            let s = 0.5 * (l + right.get(i).copied().unwrap_or(l));
            self.live[self.live_write] = (s * 32768.0).round().clamp(-32768.0, 32767.0) as i16;
            self.live_write = (self.live_write + 1) % len;
        }
    }

    // --- Scheduler -------------------------------------------------------------

    /// Runs score steps until the grain clock is `lookahead` samples past playback,
    /// or `budget` steps were taken.
    fn schedule(&mut self, horizon: f64, mut budget: usize) {
        let sr = self.domain_sr();
        let end = self.score.total_duration;
        while budget > 0 && !self.finished {
            let front = self.base as f64 + self.grain_t * sr;
            if front >= horizon {
                break;
            }
            if self.sched_t > end {
                self.finished = true;
                break;
            }
            self.step(sr);
            budget -= 1;
        }
    }

    /// One iteration of the `BreakPointsToSoundFile` loop (@ 0x10001abc).
    fn step(&mut self, sr: f64) {
        let t = self.sched_t;
        // The list read here is `GFRnd`: an onset jitter in samples, not a rate randomiser.
        let jitter = self.score.gf_rnd.find_lin(t);
        let mut gf = self.score.granular_freq(t).abs() * self.density;
        if gf == 0.0 {
            gf = 1e-5;
        }
        let period = 1.0 / gf;
        self.acc += gf * STEP_SECS;
        let n = self.acc.ceil();
        if n >= 1.0 {
            // Streaming cannot write into the past: if the grain clock fell more than
            // the look-ahead behind playback, re-anchor it to "now".
            let now = (self.read_pos - self.base as f64) / sr;
            if self.grain_t < now - LOOKAHEAD_SECS {
                self.grain_t = now;
            }
            let mut k = n;
            while k != 0.0 {
                let j = jitter * self.rng.frand() - 0.5 * jitter;
                self.put_grain(self.grain_t, j, sr);
                self.grain_t += period;
                k -= 1.0;
            }
            self.acc -= n;
        }
        self.sched_t += STEP_SECS;
    }

    /// `PutGrain` @ 0x10001cf0 + `GrainToDisk` @ 0x10004000.
    fn put_grain(&mut self, t: f64, jitter: f64, sr: f64) {
        let rng = &mut self.rng;
        let s = &self.score;
        // Same evaluation order as the original record fill.
        let gl = s.grain_length(t, rng);
        let a = s.attack(t, rng);
        let d = s.decay(t, rng);
        let g = s.gain(t, rng);
        let fm = s.freq_multiplier(t, rng);
        let b = s.balance(t, rng);
        let so = s.source_offset(t, rng);

        let mut len = (gl as i64).clamp(0, MAX_GRAIN_LEN);
        let a = a.clamp(0.0, 1.0);
        let mut d = d.clamp(0.0, 1.0);
        let g = g.clamp(0.0, 10000.0);
        let fm = fm.clamp(1e-5, 10000.0);
        let b = b.clamp(0.0, 2.0);

        let read_floor = self.read_pos as i64;
        let mut onset = self.base + ((sr * t + jitter) as i32).max(0) as i64;
        if onset < read_floor + 3 {
            onset = read_floor + 3;
        }
        // Keep the grain inside the live part of the ring.
        len = len.min(self.cleared_to + RING_LEN as i64 - onset - 1);
        if len <= 0 {
            return;
        }

        let (data, rot) = if self.use_live {
            (&self.live[..], self.live_write)
        } else {
            (&self.source.pcm16[..], 0)
        };
        let total = data.len() as i64;
        if total == 0 {
            return;
        }
        let read_len = (len as f64 * fm) as i32 as i64;
        let src_sr = sr as f32 as f64;
        let so = ((src_sr * so) as i32 as i64).clamp(0, (total - read_len).max(0));
        let total_u = total as usize;
        let sample = |i: i64| -> f64 {
            let i = so + i;
            if i < 0 || i >= total {
                0.0
            } else {
                let mut j = i as usize + rot;
                if j >= total_u {
                    j -= total_u;
                }
                data[j] as f64
            }
        };

        // `ShapeGrain2` @ 0x1000464c.
        if a + d > 1.0 {
            d = 1.0 - a;
        }
        let n_att = (a * len as f64) as u32 as i64;
        let n_dec = (d * len as f64) as u32 as i64;
        let sustain_end = len - n_dec;
        let att_step = 1000.0 / n_att as f64;
        let dec_step = 1000.0 / n_dec as f64;
        let mut env_idx = 0.0f64;

        // Constant-power pan as in `GrainToDisk`: left = sqrt(1 - b/2), right = sqrt(b/2).
        let half = b * 0.5;
        let pan_r = half.sqrt();
        let pan_l = (1.0 - half).sqrt();

        let mut pos = 0.0f64;
        for k in 0..len {
            // `GetGrain` @ 0x1000445c: linear interpolation truncated to 16 bits.
            let mut s = if fm == 1.0 {
                sample(k) as i16
            } else {
                let i = pos as u32 as i64;
                let frac = pos - i as f64;
                let s0 = sample(i);
                pos += fm;
                trunc16(frac * (sample(i + 1) - s0) + s0)
            };

            if k < n_att {
                let e = self.env.attack[(env_idx as usize).min(999)] as f64;
                s = trunc16(s as f64 * g * (e / 32767.0));
                env_idx += att_step;
                if k + 1 == n_att {
                    env_idx = 0.0;
                }
            } else if k < sustain_end {
                if g != 1.0 {
                    s = trunc16(s as f64 * g);
                }
            } else {
                let e = self.env.decay[(env_idx as usize).min(999)] as f64;
                s = trunc16(s as f64 * g * (e / 32767.0));
                env_idx += dec_step;
            }

            let idx = ((onset + k) & RING_MASK) as usize;
            self.ring_l[idx] += trunc16(pan_l * s as f64) as i32;
            self.ring_r[idx] += trunc16(pan_r * s as f64) as i32;
        }

        let end = onset + len;
        self.max_end = self.max_end.max(end);
        if self.active.len() < self.active.capacity() {
            self.active.push((onset, end));
        }
    }

    // --- Playback --------------------------------------------------------------

    #[inline]
    fn clear_behind(&mut self, upto: i64) {
        while self.cleared_to < upto {
            if self.cleared_to >= 0 {
                let i = (self.cleared_to & RING_MASK) as usize;
                self.ring_l[i] = 0;
                self.ring_r[i] = 0;
            }
            self.cleared_to += 1;
        }
    }

    #[inline]
    fn tap(&self, j: i64, wrap: bool) -> (f32, f32) {
        let i = (j & RING_MASK) as usize;
        let (l, r) = (self.ring_l[i], self.ring_r[i]);
        if wrap {
            (l as i16 as f32 / 32768.0, r as i16 as f32 / 32768.0)
        } else {
            (l as f32 / 32768.0, r as f32 / 32768.0)
        }
    }

    fn update_meter(&mut self) {
        let now = self.read_pos as i64;
        self.active.retain(|&(_, end)| end > now);
        self.active_grain_count = self.active.iter().filter(|&&(start, _)| start <= now).count();
    }

    fn maybe_restart(&mut self) {
        if self.auto_restart && self.is_finished() {
            // Like hitting Thonk again: a fresh composition, continuing seamlessly.
            let seed = self.seed_rng.next_seed();
            self.last_seed = seed;
            let dur = self.source_duration();
            self.score.regenerate(self.score_type, dur, seed);
            self.rng = MacRandom::new(seed ^ 0x5468_4f6e);
            self.base = self.read_pos as i64 + 3;
            self.start_composition();
        }
    }

    /// Renders a block of stereo audio at the host rate.
    pub fn process_block(
        &mut self,
        out_left: &mut [f32],
        out_right: &mut [f32],
        wrapping: bool,
        density: f32,
        gain_db: f32,
    ) {
        let n = out_left.len().min(out_right.len());
        self.density = density as f64;
        let sr = self.domain_sr();
        let ratio = sr / self.host_sr;
        let block = n as f64 * ratio;
        let step_samples = (sr * STEP_SECS).max(1.0);
        // 25 % faster than real time until the look-ahead is full, never in bursts.
        self.step_credit = (self.step_credit + block / step_samples * 1.25).min(4.0);
        let budget = self.step_credit.floor() as usize;
        let before = self.sched_t;
        self.schedule(self.read_pos + block + LOOKAHEAD_SECS * sr, budget);
        let taken = ((self.sched_t - before) / STEP_SECS).round();
        self.step_credit -= taken.min(self.step_credit.floor());

        let gain = 10.0f32.powf(gain_db / 20.0);
        let exact = (ratio - 1.0).abs() < 1e-12;
        for i in 0..n {
            let (l, r) = if exact {
                self.tap(self.read_pos as i64, wrapping)
            } else {
                // 4-point Hermite on the (already wrapped) signal.
                let j = self.read_pos.floor() as i64;
                let x = (self.read_pos - j as f64) as f32;
                let p = [
                    self.tap(j - 1, wrapping),
                    self.tap(j, wrapping),
                    self.tap(j + 1, wrapping),
                    self.tap(j + 2, wrapping),
                ];
                (
                    hermite(x, p[0].0, p[1].0, p[2].0, p[3].0),
                    hermite(x, p[0].1, p[1].1, p[2].1, p[3].1),
                )
            };
            if wrapping {
                out_left[i] = l * gain;
                out_right[i] = r * gain;
            } else {
                out_left[i] = (l * gain).tanh();
                out_right[i] = (r * gain).tanh();
            }
            self.read_pos += ratio;
            self.clear_behind(self.read_pos as i64 - 2);
        }

        self.update_meter();
        self.maybe_restart();
    }
}

#[inline]
fn hermite(x: f32, y0: f32, y1: f32, y2: f32, y3: f32) -> f32 {
    let c1 = 0.5 * (y2 - y0);
    let c2 = y0 - 2.5 * y1 + 2.0 * y2 - 0.5 * y3;
    let c3 = 0.5 * (y3 - y0) + 1.5 * (y1 - y2);
    ((c3 * x + c2) * x + c1) * x + y1
}

/// Renders a whole score to a 16-bit stereo file, like the original: at the input
/// file's sample rate, until the last grain has been written. In wrap mode at 0 dB
/// the samples are the exact 16-bit sums the original would have written.
pub fn offline_render_to_file<F: Fn(f32, &str)>(
    source: AudioBufferData,
    score_type: ScoreType,
    seed: Option<u32>,
    wrapping: bool,
    density_mult: f32,
    gain_db: f32,
    output_path: &std::path::Path,
    progress_callback: F,
) -> Result<u32, String> {
    let sr = source.sample_rate;
    let mut engine = GranularEngine::new(sr);
    engine.auto_restart = false;
    engine.source = source;
    engine.trigger(score_type, seed);
    engine.density = density_mult as f64;

    let mut writer = Pcm16Writer::create(output_path, sr).map_err(|e| e.to_string())?;
    let gain = 10.0f32.powf(gain_db / 20.0);
    let unity = gain_db == 0.0;
    let block = 4096usize;
    let mut frames = vec![0i16; block * 2];
    let mut blocks = 0usize;

    while !engine.is_finished() {
        engine.schedule(engine.read_pos + block as f64 + LOOKAHEAD_SECS * sr, usize::MAX);
        for f in 0..block {
            let j = engine.read_pos as i64;
            let i = (j & RING_MASK) as usize;
            let (l, r) = (engine.ring_l[i], engine.ring_r[i]);
            let (l, r) = if wrapping && unity {
                (l as i16, r as i16)
            } else if wrapping {
                (to_i16(l as i16 as f32 * gain), to_i16(r as i16 as f32 * gain))
            } else {
                (
                    to_i16((l as f32 / 32768.0 * gain).tanh() * 32768.0),
                    to_i16((r as f32 / 32768.0 * gain).tanh() * 32768.0),
                )
            };
            frames[2 * f] = l;
            frames[2 * f + 1] = r;
            engine.read_pos += 1.0;
            engine.clear_behind(engine.read_pos as i64 - 2);
        }
        writer.write_frames(&frames).map_err(|e| e.to_string())?;

        blocks += 1;
        if blocks % 64 == 0 || engine.is_finished() {
            engine.update_meter();
            let prog = if engine.finished { 1.0 } else { engine.score_progress() as f32 };
            let secs = engine.read_pos / sr;
            let status = format!(
                "Rendering {}: {:.0}% ({:.1}s) [{} layers] seed {}",
                engine.resolved_score().name(),
                prog * 100.0,
                secs,
                engine.active_grain_count,
                engine.last_seed
            );
            progress_callback(prog, &status);
        }
    }
    writer.finalize().map_err(|e| e.to_string())?;
    Ok(engine.last_seed)
}

#[inline]
fn to_i16(x: f32) -> i16 {
    x.round().clamp(-32768.0, 32767.0) as i16
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn envelope_tables_match_original_formula() {
        let e = Envelopes::new();
        assert_eq!(e.attack[0], 0);
        assert_eq!(e.decay[0], 32767);
        assert_eq!(e.attack[500], 16384);
        assert!(e.attack[999] >= 32767 - 1);
        assert!(e.decay[999] <= 2);
    }

    #[test]
    fn i32_sum_cast_equals_wrapping_adds() {
        let grains: [i16; 5] = [30000, 25000, -12000, 32767, 18000];
        let mut wrapped: i16 = 0;
        let mut sum: i32 = 0;
        for g in grains {
            wrapped = wrapped.wrapping_add(g);
            sum += g as i32;
        }
        assert_eq!(sum as i16, wrapped);
    }

    #[test]
    fn engine_renders_every_score() {
        let all = [
            ScoreType::ClassicFlowing,
            ScoreType::ClassicHectic,
            ScoreType::Pentathon,
            ScoreType::Shepard,
            ScoreType::Dis,
            ScoreType::OctavesStretch1Min,
            ScoreType::OctavesStretch5Min,
            ScoreType::PlainStretch1Min,
            ScoreType::PlainStretch5Min,
            ScoreType::Flash,
            ScoreType::Slack,
            ScoreType::Stochastic,
        ];
        for score in all {
            let mut e = GranularEngine::new(48000.0);
            e.trigger(score, Some(1234));
            let mut l = vec![0.0f32; 512];
            let mut r = vec![0.0f32; 512];
            let mut energy = 0.0f32;
            for _ in 0..(48000 * 6 / 512) {
                e.process_block(&mut l, &mut r, true, 1.0, 0.0);
                assert!(l.iter().chain(r.iter()).all(|x| x.is_finite()));
                energy += l.iter().map(|x| x * x).sum::<f32>();
            }
            assert!(energy > 0.0, "{score:?} produced silence");
        }
    }

    #[test]
    fn fixed_seed_is_reproducible() {
        let run = || {
            let mut e = GranularEngine::new(44100.0);
            e.trigger(ScoreType::ClassicHectic, Some(77));
            let mut l = vec![0.0f32; 1024];
            let mut r = vec![0.0f32; 1024];
            let mut acc = 0.0f64;
            for _ in 0..100 {
                e.process_block(&mut l, &mut r, true, 1.0, 0.0);
                acc += l.iter().map(|&x| x as f64).sum::<f64>();
            }
            acc
        };
        assert_eq!(run(), run());
    }

    /// Worst-case block times. Run with:
    /// `cargo test --release --lib rt_budget -- --ignored --nocapture`
    #[test]
    #[ignore]
    fn rt_budget() {
        use std::time::Instant;
        let sr = 48000.0;
        let block = 128usize;
        let budget_us = block as f64 / sr * 1e6;
        let mut l = vec![0.0f32; block];
        let mut r = vec![0.0f32; block];
        for score in [
            ScoreType::ClassicHectic,
            ScoreType::Flash,
            ScoreType::Pentathon,
            ScoreType::Shepard,
            ScoreType::PlainStretch1Min,
        ] {
            let mut e = GranularEngine::new(sr);
            let t0 = Instant::now();
            e.trigger(score, Some(99));
            let trig = t0.elapsed().as_secs_f64() * 1e6;
            let mut worst = 0.0f64;
            let mut total = 0.0f64;
            let mut over = 0usize;
            let blocks = (sr as usize * 60) / block;
            for _ in 0..blocks {
                let t = Instant::now();
                e.process_block(&mut l, &mut r, true, 1.0, 0.0);
                let us = t.elapsed().as_secs_f64() * 1e6;
                worst = worst.max(us);
                total += us;
                if us > budget_us * 0.5 {
                    over += 1;
                }
            }
            println!(
                "{:<24} trigger {:7.0} us | block avg {:6.1} us ({:4.1}% CPU) worst {:7.0} us, {} blocks > 50% (budget {:.0} us)",
                score.name(),
                trig,
                total / blocks as f64,
                total / blocks as f64 / budget_us * 100.0,
                worst,
                over,
                budget_us
            );
        }
    }
}
