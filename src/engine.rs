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

use crate::aiff::{AudioBufferData, AudioWriter, ExportFormat};
use crate::params::GRAIN_TRAIL;
use crate::score::{trunc16, Length, MacRandom, ScoreState, ScoreType};
#[cfg(test)]
use crate::score::LengthMode;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;
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
/// Fade applied when a Stretch / Cut composition reaches its exact end.
const END_FADE_SECS: f64 = 0.03;
/// With "skip silences": the most one grain may advance the grain clock.
const MAX_GAP_SECS: f64 = 0.5;
/// Mixed into the seed for the engine's own generator (grain-level randomness).
const ENGINE_SEED_MIX: u32 = 0x5468_4f6e;
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

/// A recently mixed grain, for the waveform display.
#[derive(Clone, Copy, Default)]
pub struct GrainMark {
    /// Read position in the source, 0..1.
    pub source: f32,
    /// Balance 0 (left) .. 1 (right).
    pub balance: f32,
}

pub struct GranularEngine {
    // Source material, as 16-bit PCM like the original.
    source: Arc<AudioBufferData>,
    live: Vec<i16>,
    live_write: usize,
    use_live: bool,
    host_sr: f64,

    // Composition.
    score: ScoreState,
    score_type: ScoreType,
    length: Length,
    seed: u32,
    seed_rng: MacRandom,
    rng: MacRandom,

    // Scheduler state (`BreakPointsToSoundFile`).
    sched_t: f64,
    grain_t: f64,
    acc: f64,
    finished: bool,
    density: f64,
    skip_silence: bool,
    /// Absolute ring position (domain samples) of composition time 0.
    base: i64,
    /// Absolute position of the exact end (Stretch / Cut), if any.
    end_pos: Option<i64>,

    // Accumulation ring and playback.
    ring_l: Vec<i32>,
    ring_r: Vec<i32>,
    read_pos: f64,
    cleared_to: i64,
    max_end: i64,
    env: Envelopes,
    playing: bool,
    /// Fractional scheduler steps this block may take: work is spread evenly over
    /// blocks instead of bursting when the look-ahead refills.
    step_credit: f64,

    active: Vec<(i64, i64)>,
    pub active_grain_count: usize,
    trail: [GrainMark; GRAIN_TRAIL],
    trail_head: usize,
    /// Chain a new composition when one ends (audition); off when following the host.
    pub auto_restart: bool,
}

impl GranularEngine {
    pub fn new(host_sr: f64) -> Self {
        let seed = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .map(|d| d.as_secs() as u32 ^ d.subsec_nanos())
            .unwrap_or(0x4170);
        let mut engine = Self {
            source: Arc::new(AudioBufferData::default()),
            live: vec![0; (host_sr * LIVE_SECS) as usize],
            live_write: 0,
            use_live: false,
            host_sr,
            score: ScoreState::new(),
            score_type: ScoreType::ClassicFlowing,
            length: Length::default(),
            seed: 1,
            seed_rng: MacRandom::new(seed),
            rng: MacRandom::new(1),
            sched_t: 0.0,
            grain_t: 0.0,
            acc: 0.0,
            finished: true,
            density: 1.0,
            skip_silence: false,
            base: 0,
            end_pos: None,
            ring_l: vec![0; RING_LEN],
            ring_r: vec![0; RING_LEN],
            read_pos: 0.0,
            cleared_to: -4,
            max_end: 0,
            env: Envelopes::new(),
            playing: false,
            step_credit: 0.0,
            active: Vec::with_capacity(16384),
            active_grain_count: 0,
            trail: [GrainMark::default(); GRAIN_TRAIL],
            trail_head: 0,
            auto_restart: true,
        };
        engine.trigger(ScoreType::ClassicFlowing, None);
        engine
    }

    // --- Configuration ---------------------------------------------------------

    /// Sample rate of the synthesis domain: the source file's rate, or the host's
    /// when granulating live input.
    pub fn domain_sr(&self) -> f64 {
        if self.use_live || self.source.sample_rate <= 0.0 {
            self.host_sr
        } else {
            self.source.sample_rate
        }
    }

    pub fn source_duration(&self) -> f64 {
        if self.use_live {
            self.live.len() as f64 / self.host_sr
        } else {
            self.source.duration_secs()
        }
    }

    pub fn source(&self) -> &Arc<AudioBufferData> {
        &self.source
    }

    /// Host rate change. Reallocates the live buffer, so call it off the audio path
    /// (`reset`), never from `process`.
    pub fn set_host_sample_rate(&mut self, sr: f64) {
        if sr != self.host_sr {
            self.host_sr = sr;
            self.live = vec![0; (sr * LIVE_SECS) as usize];
            self.live_write = 0;
        }
        self.restart();
    }

    /// Swaps in a new source and returns the previous one (so the caller can drop it
    /// off the audio thread). Regenerates the score for the new source length,
    /// keeping the seed and the play position.
    pub fn set_source(&mut self, data: Arc<AudioBufferData>) -> Arc<AudioBufferData> {
        let old = std::mem::replace(&mut self.source, data);
        self.rebuild_keep_position();
        old
    }

    pub fn set_live(&mut self, live: bool) {
        if live != self.use_live {
            self.use_live = live;
            self.rebuild_keep_position();
        }
    }

    /// Changes the composition length (keeps seed and position).
    pub fn set_length(&mut self, length: Length) {
        if length != self.length {
            self.length = length;
            self.rebuild_keep_position();
        }
    }

    pub fn set_skip_silence(&mut self, on: bool) {
        self.skip_silence = on;
    }

    pub fn set_density(&mut self, density: f64) {
        self.density = density;
    }

    fn rebuild_keep_position(&mut self) {
        let pos = self.position_secs();
        let dur = self.source_duration();
        self.score
            .regenerate_with(self.score_type, dur, self.seed, self.length);
        self.seek(pos.min(self.score.total_duration));
    }

    // --- Transport -------------------------------------------------------------

    /// Hit "Thonk": regenerate the score (seeded like `GetDateTime(&qd.randSeed)`,
    /// or with a fixed seed for recall) and start it from the beginning.
    pub fn trigger(&mut self, score_type: ScoreType, seed: Option<u32>) {
        let seed = seed.unwrap_or_else(|| self.seed_rng.next_seed());
        self.seed = seed;
        self.score_type = score_type;
        let dur = self.source_duration();
        self.score.regenerate_with(score_type, dur, seed, self.length);
        self.seek(0.0);
    }

    /// Back to the start of the current composition.
    pub fn restart(&mut self) {
        let score_type = self.score_type;
        self.trigger(score_type, Some(self.seed));
    }

    pub fn play(&mut self) {
        self.playing = true;
    }

    /// Stops and rewinds to the start of the composition (same seed), like Live's Stop.
    pub fn stop(&mut self) {
        self.playing = false;
        self.seek(0.0);
    }

    pub fn is_playing(&self) -> bool {
        self.playing
    }

    /// Jumps to composition time `t` (seconds). Deterministic: the same seed and
    /// position always give the same audio, so DAW bounces are repeatable.
    pub fn seek(&mut self, t: f64) {
        let sr = self.domain_sr();
        let now = self.read_pos as i64;
        // Silence whatever was already mixed ahead of playback.
        for j in now..self.max_end.max(now) {
            let i = (j & RING_MASK) as usize;
            self.ring_l[i] = 0;
            self.ring_r[i] = 0;
        }
        self.active.clear();
        self.step_credit = 0.0;
        self.max_end = now;

        let t = t.max(0.0);
        let k = (t / STEP_SECS).floor() as u64;
        self.sched_t = k as f64 * STEP_SECS;
        self.grain_t = self.sched_t;
        self.acc = 0.0;
        let mix = (k as u32).wrapping_mul(0x9e37_79b9) ^ ((k >> 32) as u32);
        self.rng = MacRandom::new(self.seed ^ ENGINE_SEED_MIX ^ mix);
        self.finished = self.sched_t > self.score.total_duration;
        // Composition time `t` lands exactly "now" (offline renders start at 0).
        self.base = if now == 0 && k == 0 { 0 } else { now + 3 - (t * sr) as i64 };
        self.end_pos = self
            .score
            .hard_end
            .then(|| self.base + (self.score.total_duration * sr) as i64);
    }

    // --- Status ----------------------------------------------------------------

    pub fn seed(&self) -> u32 {
        self.seed
    }

    /// The score actually playing (resolves "ztochazt!k" to the chosen score).
    pub fn resolved_score(&self) -> ScoreType {
        self.score.resolved
    }

    pub fn total_duration(&self) -> f64 {
        self.score.total_duration
    }

    pub fn position_secs(&self) -> f64 {
        ((self.read_pos - self.base as f64) / self.domain_sr()).max(0.0)
    }

    pub fn score_progress(&self) -> f64 {
        (self.sched_t / self.score.total_duration).clamp(0.0, 1.0)
    }

    pub fn is_finished(&self) -> bool {
        match self.end_pos {
            Some(end) => self.read_pos as i64 >= end,
            None => self.finished && self.read_pos as i64 >= self.max_end,
        }
    }

    /// Recent grains, oldest first.
    pub fn grain_trail(&self) -> impl Iterator<Item = GrainMark> + '_ {
        (0..GRAIN_TRAIL).map(move |i| self.trail[(self.trail_head + i) % GRAIN_TRAIL])
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

    /// Runs score steps until the grain clock is past `horizon`, or `budget` steps
    /// were taken. With "skip silences", steps that produce nothing are free.
    fn schedule(&mut self, horizon: f64, mut budget: usize) -> usize {
        let sr = self.domain_sr();
        let end = self.score.total_duration;
        let mut taken = 0;
        let mut free = 2000;
        while budget > 0 && !self.finished {
            let front = self.base as f64 + self.grain_t * sr;
            if front >= horizon {
                break;
            }
            if self.sched_t > end {
                self.finished = true;
                break;
            }
            let emitted = self.step(sr);
            if self.skip_silence && emitted == 0 && free > 0 {
                free -= 1;
            } else {
                budget -= 1;
                taken += 1;
            }
        }
        taken
    }

    /// One iteration of the `BreakPointsToSoundFile` loop (@ 0x10001abc).
    fn step(&mut self, sr: f64) -> usize {
        let t = self.sched_t;
        // The list read here is `GFRnd`: an onset jitter in samples, not a rate randomiser.
        let jitter = self.score.gf_rnd.find_lin(t);
        let mut gf = self.score.granular_freq(t).abs() * self.density;
        if gf == 0.0 {
            gf = 1e-5;
        }
        let mut period = 1.0 / gf;
        if self.skip_silence {
            period = period.min(MAX_GAP_SECS);
        }
        self.acc += gf * STEP_SECS;
        let n = self.acc.ceil();
        let mut emitted = 0;
        if n >= 1.0 {
            // Streaming cannot write into the past: if the grain clock fell more than
            // the look-ahead behind playback, re-anchor it to "now".
            let now = (self.read_pos - self.base as f64) / sr;
            let slack = if self.skip_silence { 0.05 } else { LOOKAHEAD_SECS };
            if self.grain_t < now - slack {
                self.grain_t = now;
            }
            let mut k = n;
            while k != 0.0 {
                let j = jitter * self.rng.frand() - 0.5 * jitter;
                self.put_grain(self.grain_t, j, sr);
                self.grain_t += period;
                k -= 1.0;
                emitted += 1;
            }
            self.acc -= n;
        }
        self.sched_t += STEP_SECS;
        emitted
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
        if let Some(end) = self.end_pos {
            if onset >= end {
                return;
            }
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
        self.trail[self.trail_head] = GrainMark {
            source: (so as f64 / total as f64) as f32,
            balance: half as f32,
        };
        self.trail_head = (self.trail_head + 1) % GRAIN_TRAIL;
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

    /// Gain of the exact-end fade at absolute position `pos` (1 when there is none).
    #[inline]
    fn end_gain(&self, pos: f64, fade: f64) -> f32 {
        match self.end_pos {
            Some(end) => ((end as f64 - pos) / fade).clamp(0.0, 1.0) as f32,
            None => 1.0,
        }
    }

    fn update_meter(&mut self) {
        let now = self.read_pos as i64;
        self.active.retain(|&(_, end)| end > now);
        self.active_grain_count = self.active.iter().filter(|&&(start, _)| start <= now).count();
    }

    fn maybe_restart(&mut self) {
        if self.is_finished() {
            if self.auto_restart {
                // Like hitting Thonk again: a fresh composition, continuing seamlessly.
                let score_type = self.score_type;
                self.trigger(score_type, None);
            } else {
                self.playing = false;
            }
        }
    }

    /// Renders a block of stereo audio at the host rate. Silent while stopped.
    pub fn process_block(
        &mut self,
        out_left: &mut [f32],
        out_right: &mut [f32],
        wrapping: bool,
        gain_db: f32,
    ) {
        let n = out_left.len().min(out_right.len());
        if !self.playing {
            out_left[..n].fill(0.0);
            out_right[..n].fill(0.0);
            self.active_grain_count = 0;
            return;
        }
        let sr = self.domain_sr();
        let ratio = sr / self.host_sr;
        let block = n as f64 * ratio;
        let step_samples = (sr * STEP_SECS).max(1.0);
        // 25 % faster than real time until the look-ahead is full, never in bursts.
        self.step_credit = (self.step_credit + block / step_samples * 1.25).min(4.0);
        let budget = self.step_credit.floor() as usize;
        let taken = self.schedule(self.read_pos + block + LOOKAHEAD_SECS * sr, budget);
        self.step_credit -= taken as f64;

        let gain = 10.0f32.powf(gain_db / 20.0);
        let fade = END_FADE_SECS * sr;
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
            let fade_gain = self.end_gain(self.read_pos, fade);
            if wrapping {
                out_left[i] = l * gain * fade_gain;
                out_right[i] = r * gain * fade_gain;
            } else {
                out_left[i] = (l * gain).tanh() * fade_gain;
                out_right[i] = (r * gain).tanh() * fade_gain;
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

/// Everything an offline render needs.
#[derive(Clone)]
pub struct RenderOptions {
    pub score: ScoreType,
    /// None = a fresh random composition.
    pub seed: Option<u32>,
    pub wrapping: bool,
    /// Multiplier (1.0 = 100 %).
    pub density: f32,
    pub gain_db: f32,
    pub length: Length,
    pub skip_silence: bool,
    pub format: ExportFormat,
    /// Scale the result so its peak sits at -0.3 dBFS (two passes, same seed).
    pub normalize: bool,
}

impl Default for RenderOptions {
    fn default() -> Self {
        Self {
            score: ScoreType::ClassicFlowing,
            seed: None,
            wrapping: true,
            density: 1.0,
            gain_db: 0.0,
            length: Length::default(),
            skip_silence: false,
            format: ExportFormat::Int16,
            normalize: false,
        }
    }
}

/// Error from an offline render.
#[derive(Debug, PartialEq)]
pub enum RenderError {
    Cancelled,
    Io(String),
}

impl std::fmt::Display for RenderError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::Cancelled => write!(f, "cancelled"),
            Self::Io(e) => write!(f, "{e}"),
        }
    }
}

/// Renders a whole composition to disk, like the original: at the input file's
/// sample rate, until the last grain has been written (Original length) or to the
/// exact length (Stretch / Cut). In wrap mode at 0 dB, 16-bit output contains the
/// exact sums the original would have written.
///
/// `progress` receives (0..1, status line). Setting `cancel` stops the render and
/// deletes the partial file. Returns the seed used.
pub fn offline_render_to_file<F: FnMut(f32, &str)>(
    source: Arc<AudioBufferData>,
    opts: &RenderOptions,
    output_path: &std::path::Path,
    cancel: &AtomicBool,
    mut progress: F,
) -> Result<u32, RenderError> {
    let mut seed_rng = MacRandom::new(
        std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .map(|d| d.subsec_nanos() ^ d.as_secs() as u32)
            .unwrap_or(7),
    );
    // Both passes of a normalised render must hear the same composition.
    let seed = opts.seed.unwrap_or_else(|| seed_rng.next_seed());

    let scale = if opts.normalize {
        let peak = render_pass(&source, opts, seed, None, cancel, &mut |p, m| {
            progress(p * 0.5, &format!("Analysing — {m}"))
        })?;
        if peak > 0.0 { 0.966 / peak } else { 1.0 }
    } else {
        1.0
    };

    let mut writer = AudioWriter::create(output_path, source.sample_rate, opts.format)
        .map_err(RenderError::Io)?;
    let result = render_pass(
        &source,
        opts,
        seed,
        Some((&mut writer, scale)),
        cancel,
        &mut |p, m| {
            let p = if opts.normalize { 0.5 + p * 0.5 } else { p };
            progress(p, m)
        },
    );
    match result {
        Ok(_) => {
            writer.finalize().map_err(RenderError::Io)?;
            Ok(seed)
        }
        Err(e) => {
            drop(writer);
            let _ = std::fs::remove_file(output_path);
            Err(e)
        }
    }
}

/// One pass over the composition. Writes (scaled) frames if a writer is given;
/// returns the peak of the unscaled output.
fn render_pass(
    source: &Arc<AudioBufferData>,
    opts: &RenderOptions,
    seed: u32,
    mut out: Option<(&mut AudioWriter, f32)>,
    cancel: &AtomicBool,
    progress: &mut dyn FnMut(f32, &str),
) -> Result<f32, RenderError> {
    let sr = source.sample_rate;
    let mut engine = GranularEngine::new(sr);
    engine.auto_restart = false;
    engine.source = source.clone();
    engine.length = opts.length;
    engine.skip_silence = opts.skip_silence;
    engine.density = opts.density as f64;
    engine.trigger(opts.score, Some(seed));

    let gain = 10.0f32.powf(opts.gain_db / 20.0);
    let fade = END_FADE_SECS * sr;
    let block = 4096usize;
    let mut frames = vec![0f32; block * 2];
    let mut peak = 0.0f32;
    let mut blocks = 0usize;

    while !engine.is_finished() {
        if cancel.load(Ordering::Relaxed) {
            return Err(RenderError::Cancelled);
        }
        engine.schedule(engine.read_pos + block as f64 + LOOKAHEAD_SECS * sr, usize::MAX);
        let mut n = block;
        if let Some(end) = engine.end_pos {
            n = n.min((end - engine.read_pos as i64).max(0) as usize);
        }
        for f in 0..n {
            let j = engine.read_pos as i64;
            let i = (j & RING_MASK) as usize;
            let (l, r) = (engine.ring_l[i], engine.ring_r[i]);
            let g = engine.end_gain(engine.read_pos, fade);
            let (l, r) = if opts.wrapping {
                (
                    l as i16 as f32 / 32768.0 * gain * g,
                    r as i16 as f32 / 32768.0 * gain * g,
                )
            } else {
                (
                    (l as f32 / 32768.0 * gain).tanh() * g,
                    (r as f32 / 32768.0 * gain).tanh() * g,
                )
            };
            peak = peak.max(l.abs()).max(r.abs());
            frames[2 * f] = l;
            frames[2 * f + 1] = r;
            engine.read_pos += 1.0;
            engine.clear_behind(engine.read_pos as i64 - 2);
        }
        if let Some((w, scale)) = out.as_mut() {
            if *scale != 1.0 {
                for x in &mut frames[..2 * n] {
                    *x *= *scale;
                }
            }
            w.write_frames(&frames[..2 * n]).map_err(RenderError::Io)?;
        }
        if n == 0 {
            break;
        }

        blocks += 1;
        if blocks % 64 == 0 || engine.is_finished() {
            engine.update_meter();
            let total = engine.score.total_duration;
            let prog = match engine.end_pos {
                Some(_) => (engine.position_secs() / total).min(1.0) as f32,
                None if engine.finished => 1.0,
                None => engine.score_progress() as f32,
            };
            let status = format!(
                "{} · {:.0}% · {:.1}s · {} layers · seed {}",
                engine.resolved_score().name(),
                prog * 100.0,
                engine.read_pos / sr,
                engine.active_grain_count,
                seed
            );
            progress(prog, &status);
        }
    }
    Ok(peak)
}

#[cfg(test)]
mod tests {
    use super::*;

    fn demo_engine(sr: f64) -> GranularEngine {
        let mut e = GranularEngine::new(sr);
        e.set_source(Arc::new(AudioBufferData::demo()));
        e
    }

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
    fn starts_stopped_and_silent() {
        let mut e = demo_engine(48000.0);
        let mut l = vec![1.0f32; 256];
        let mut r = vec![1.0f32; 256];
        e.process_block(&mut l, &mut r, true, 0.0);
        assert!(l.iter().chain(r.iter()).all(|&x| x == 0.0));
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
            let mut e = demo_engine(48000.0);
            e.trigger(score, Some(1234));
            e.play();
            let mut l = vec![0.0f32; 512];
            let mut r = vec![0.0f32; 512];
            let mut energy = 0.0f32;
            for _ in 0..(48000 * 6 / 512) {
                e.process_block(&mut l, &mut r, true, 0.0);
                assert!(l.iter().chain(r.iter()).all(|x| x.is_finite()));
                energy += l.iter().map(|x| x * x).sum::<f32>();
            }
            assert!(energy > 0.0, "{score:?} produced silence");
        }
    }

    fn run_from(seek: Option<f64>) -> Vec<f32> {
        let mut e = demo_engine(44100.0);
        e.trigger(ScoreType::ClassicHectic, Some(77));
        if let Some(t) = seek {
            e.seek(t);
        }
        e.play();
        let mut l = vec![0.0f32; 1024];
        let mut r = vec![0.0f32; 1024];
        let mut out = Vec::new();
        for _ in 0..100 {
            e.process_block(&mut l, &mut r, true, 0.0);
            out.extend_from_slice(&l);
        }
        out
    }

    #[test]
    fn fixed_seed_is_reproducible() {
        assert_eq!(run_from(None), run_from(None));
    }

    #[test]
    fn seek_is_deterministic() {
        let a = run_from(Some(300.0));
        assert_eq!(a, run_from(Some(300.0)));
        assert_ne!(a, run_from(None));
    }

    #[test]
    fn stretch_render_has_exact_length() {
        let src = Arc::new(AudioBufferData::demo());
        let dir = std::env::temp_dir().join(format!("thonk-test-{}", std::process::id()));
        std::fs::create_dir_all(&dir).unwrap();
        let path = dir.join("stretch.wav");
        let opts = RenderOptions {
            score: ScoreType::ClassicHectic,
            seed: Some(5),
            length: Length {
                mode: LengthMode::Stretch,
                secs: 12.0,
            },
            ..Default::default()
        };
        offline_render_to_file(src, &opts, &path, &AtomicBool::new(false), |_, _| {}).unwrap();
        let reader = hound::WavReader::open(&path).unwrap();
        assert_eq!(reader.duration(), (12.0 * 44100.0) as u32);
        let _ = std::fs::remove_dir_all(&dir);
    }

    #[test]
    fn normalised_render_peaks_near_full_scale_and_cancel_cleans_up() {
        let src = Arc::new(AudioBufferData::demo());
        let dir = std::env::temp_dir().join(format!("thonk-norm-{}", std::process::id()));
        std::fs::create_dir_all(&dir).unwrap();
        let path = dir.join("n.wav");
        let opts = RenderOptions {
            score: ScoreType::ClassicHectic,
            seed: Some(9),
            length: Length {
                mode: LengthMode::Cut,
                secs: 10.0,
            },
            format: ExportFormat::Float32,
            normalize: true,
            ..Default::default()
        };
        offline_render_to_file(src.clone(), &opts, &path, &AtomicBool::new(false), |_, _| {})
            .unwrap();
        let mut reader = hound::WavReader::open(&path).unwrap();
        let peak = reader
            .samples::<f32>()
            .map(|s| s.unwrap().abs())
            .fold(0.0f32, f32::max);
        assert!((peak - 0.966).abs() < 1e-3, "peak {peak}");

        std::fs::remove_file(&path).unwrap();
        let cancelled =
            offline_render_to_file(src, &opts, &path, &AtomicBool::new(true), |_, _| {});
        assert_eq!(cancelled, Err(RenderError::Cancelled));
        assert!(!path.exists());
        let _ = std::fs::remove_dir_all(&dir);
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
            let mut e = demo_engine(sr);
            let t0 = Instant::now();
            e.trigger(score, Some(99));
            let trig = t0.elapsed().as_secs_f64() * 1e6;
            e.play();
            let mut worst = 0.0f64;
            let mut total = 0.0f64;
            let mut over = 0usize;
            let blocks = (sr as usize * 60) / block;
            for _ in 0..blocks {
                let t = Instant::now();
                e.process_block(&mut l, &mut r, true, 0.0);
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
