//! Score generators and breakpoint parameter interpolation.
//!
//! A 1:1 translation of the score functions of thOnk_0+2 (Audio Ease, 1996-1999),
//! recovered from the PowerPC PEF binary. Every constant below was read from the
//! TOC (data section) of the original executable; `reverse_engineering/README.md`
//! documents where each one lives and how the call arguments were decoded.

use truce::prelude::*;

/// All 12 Th0nk composition methods (scores), in the order of the original popup.
#[derive(ParamEnum, Default, Debug)]
pub enum ScoreType {
    #[default]
    #[name = "Classic Flowing"]
    ClassicFlowing,

    #[name = "Classic Hectic"]
    ClassicHectic,

    #[name = "Pentathon"]
    Pentathon,

    #[name = "Shepard"]
    Shepard,

    #[name = "Dis"]
    Dis,

    #[name = "Octaves Stretch (1 min)"]
    OctavesStretch1Min,

    #[name = "Octaves Stretch (5 min)"]
    OctavesStretch5Min,

    #[name = "Plain Stretch (1 min)"]
    PlainStretch1Min,

    #[name = "Plain Stretch (5 min)"]
    PlainStretch5Min,

    #[name = "Flash"]
    Flash,

    #[name = "Slack"]
    Slack,

    #[name = "ztochazt!k = akadem!k wank"]
    Stochastic,
}

impl ScoreType {
    /// The score descriptions, verbatim from the original data section.
    pub fn description(&self) -> &'static str {
        match self {
            ScoreType::ClassicFlowing => {
                "'Classic Flowing', from the original Thonk, provides slowly evolving narrative mini-compositions. Try 10 seconds of hi-density fast paced input."
            }
            ScoreType::ClassicHectic => {
                "'Classic Hectic', from the original Thonk, moves around fast. You can clip short events from the output for FX samples. Give it hot leveled audio."
            }
            ScoreType::Pentathon => {
                "'Pentathon' gives a two voice, brownian walk, pentatonic counterpoint. Feed it one note or a very short melody for solid tonal results."
            }
            ScoreType::Shepard => {
                "Roger 'Shepard' invented circular pitch. This stream keeps going down in micro and macro pitch and in speed."
            }
            ScoreType::Dis => {
                "'Dis' gives dark textures while moving all granular parameters around. Feed it not too fast paced audio to make its output even larger."
            }
            ScoreType::OctavesStretch1Min => {
                "This 'octaves stretch' fits the input to 1 minute, while octaves fade in and out. Direction and tonality of the input are preserved."
            }
            ScoreType::OctavesStretch5Min => {
                "This 'octaves stretch' fits the input to 5 minutes, while octaves fade in and out. Direction and tonality of the input are preserved."
            }
            ScoreType::PlainStretch1Min => {
                "Plain stretch to 1 minute only widens the image and fits input to 1 minute. Output is a thick texture where no pitch shifting is applied."
            }
            ScoreType::PlainStretch5Min => {
                "Plain stretch to 5 minutes only widens the image and fits input to 5 minutes. Output is a thick texture where no pitch shifting is applied."
            }
            ScoreType::Flash => {
                "Derived from Classic Hectic, Flash realy sprints through your input. And it likes its breakfast hot and dense."
            }
            ScoreType::Slack => {
                "Slack is not the hand of God. Slack is the way the wind swirls, and the dust settles, eons after God has passed by."
            }
            ScoreType::Stochastic => {
                "ztochazt!k = akadem!k wank: picks one of the scores at random on every Thonk. Yeah, right... You Thonk."
            }
        }
    }

    /// Rendering duration in seconds for this score method.
    pub fn duration_secs(&self) -> f64 {
        match self {
            ScoreType::OctavesStretch1Min | ScoreType::PlainStretch1Min => 60.0,
            ScoreType::OctavesStretch5Min | ScoreType::PlainStretch5Min => 300.0,
            _ => 1200.0,
        }
    }
}

/// How the composition length relates to the score's own length.
#[derive(ParamEnum, Default, Debug)]
pub enum LengthMode {
    /// The score's own length (20 min, or 1 / 5 min for the stretches) and the
    /// original ending: the file runs until the last grain.
    #[default]
    #[name = "Original"]
    Original,
    /// The whole score is time-scaled into the chosen length.
    #[name = "Stretch"]
    Stretch,
    /// The score plays at its own pace and stops at the chosen length.
    #[name = "Cut"]
    Cut,
}

/// Length settings for a composition.
#[derive(Clone, Copy, Debug, PartialEq)]
pub struct Length {
    pub mode: LengthMode,
    pub secs: f64,
}

impl Default for Length {
    fn default() -> Self {
        Self {
            mode: LengthMode::Original,
            secs: 1200.0,
        }
    }
}

impl Length {
    /// Effective length for a score whose own length is `native` seconds.
    pub fn effective(&self, native: f64) -> f64 {
        match self.mode {
            LengthMode::Original => native,
            LengthMode::Stretch => self.secs,
            LengthMode::Cut => self.secs.min(native),
        }
    }
}

/// The QuickDraw `Random()` trap: Park-Miller minimal standard generator on
/// `qd.randSeed`, returning the low 16 bits, with 0x8000 mapped to 0.
/// The original seeds it with `GetDateTime(&qd.randSeed)` at the start of every score.
pub struct MacRandom {
    seed: u32,
}

impl MacRandom {
    pub fn new(seed: u32) -> Self {
        let seed = seed % 0x7fff_ffff;
        Self {
            seed: if seed == 0 { 0x2f6b_6f55 } else { seed },
        }
    }

    /// `Random()`: uniformly distributed in -32767..=32767.
    pub fn random(&mut self) -> i16 {
        // seed * 16807 mod (2^31 - 1), reduced without a division.
        let x = self.seed as u64 * 16807;
        let mut y = (x & 0x7fff_ffff) + (x >> 31);
        if y >= 0x7fff_ffff {
            y -= 0x7fff_ffff;
        }
        self.seed = y as u32;
        let r = self.seed as u16 as i16;
        if r == i16::MIN { 0 } else { r }
    }

    /// `FRand()` @ 0x10004a44: `|Random()/32768 + Random()| / 32768`, in [0, 1).
    pub fn frand(&mut self) -> f64 {
        let r1 = self.random() as f64 / 32768.0;
        let r2 = self.random() as f64;
        ((r1 + r2) / 32768.0).abs()
    }

    /// `Rand(n)` @ 0x10005004: `(Random() & 0xffff) / (0x10000 / n) + 1`, i.e. 1..=n
    /// (n + 1 on the rare top values, exactly like the original).
    pub fn rand(&mut self, n: i32) -> i32 {
        let r = (self.random() as u16) as i32;
        r / (0x10000 / n) + 1
    }

    /// A fresh seed in 1..=999999 (the range of the Seed parameter, so any
    /// composition heard can be locked and recalled).
    pub fn next_seed(&mut self) -> u32 {
        let r = ((self.random() as u16 as u32) << 16) | (self.random() as u16 as u32);
        r % 999_999 + 1
    }
}

/// `(short)(int)x` on PowerPC: `fctiwz` saturates to i32, the store truncates to 16 bits.
#[inline(always)]
pub fn trunc16(x: f64) -> i16 {
    (x as i32) as i16
}

/// The time of the breakpoint every list receives from `SetDefaultValues` (1e9 s).
const DEFAULT_TIME: f64 = 1.0e9;

/// A breakpoint. `seq` reproduces the original linked-list order for equal times:
/// `PutBPInList` inserts a node *before* existing nodes with the same time.
#[derive(Clone, Copy, Debug)]
struct BreakPoint {
    time: f64,
    value: f64,
    seq: u32,
}

/// One breakpoint list (`InitBP` / `PutBPInList` / `FindLinParm` / `FindExpParm`).
#[derive(Clone, Debug, Default)]
pub struct BreakPointList {
    points: Vec<BreakPoint>,
}

impl BreakPointList {
    fn with_capacity(n: usize) -> Self {
        Self {
            points: Vec::with_capacity(n),
        }
    }

    fn set(&mut self, value: f64, time: f64, seq: &mut u32) {
        *seq += 1;
        self.points.push(BreakPoint {
            time,
            value,
            seq: *seq,
        });
    }

    fn finish(&mut self) {
        self.points
            .sort_unstable_by(|a, b| a.time.total_cmp(&b.time).then(b.seq.cmp(&a.seq)));
    }

    pub fn len(&self) -> usize {
        self.points.len()
    }

    pub fn is_empty(&self) -> bool {
        self.points.is_empty()
    }

    /// Node returned by `GetInsertBP(t)`: the last node with time < t, or the head.
    #[inline]
    fn locate(&self, time: f64) -> usize {
        let i = self.points.partition_point(|p| p.time < time);
        i.saturating_sub(1)
    }

    /// `FindLinParm` @ 0x100018fc.
    pub fn find_lin(&self, time: f64) -> f64 {
        let Some(head) = self.points.first() else {
            return 0.0;
        };
        if time < head.time {
            return head.value;
        }
        let i = self.locate(time);
        let p = self.points[i];
        let Some(n) = self.points.get(i + 1) else {
            return p.value;
        };
        let mut dt = n.time - p.time;
        if dt == 0.0 {
            dt = 0.01;
        }
        (time - p.time) / dt * (n.value - p.value) + p.value
    }

    /// `FindExpParm` @ 0x100019b8: logarithmic interpolation, zeros replaced by 1e-4.
    pub fn find_exp(&self, time: f64) -> f64 {
        let Some(head) = self.points.first() else {
            return 0.0;
        };
        if time < head.time {
            return head.value;
        }
        let i = self.locate(time);
        let p = self.points[i];
        let v0 = if p.value == 0.0 { 1e-4 } else { p.value };
        let Some(n) = self.points.get(i + 1) else {
            return v0;
        };
        let v1 = if n.value == 0.0 { 1e-4 } else { n.value };
        let mut dt = n.time - p.time;
        if dt == 0.0 {
            dt = 0.01;
        }
        let (l0, l1) = (v0.ln(), v1.ln());
        ((time - p.time) / dt * (l1 - l0) + l0).exp()
    }
}

/// A node of the statistical four-voice transposer (`SetFM`, 0x50-byte record):
/// four pitches in semitones and four 16-bit weights (percent).
#[derive(Clone, Copy, Debug)]
struct FmNode {
    time: f64,
    pitch: [f64; 4],
    weight: [f64; 4],
    seq: u32,
}

#[derive(Clone, Debug, Default)]
pub struct FmList {
    nodes: Vec<FmNode>,
}

impl FmList {
    fn with_capacity(n: usize) -> Self {
        Self {
            nodes: Vec::with_capacity(n),
        }
    }

    fn set(&mut self, pitch: [f64; 4], time: f64, weight: [i32; 4], seq: &mut u32) {
        *seq += 1;
        self.nodes.push(FmNode {
            time,
            pitch,
            weight: weight.map(|w| w as i16 as f64),
            seq: *seq,
        });
    }

    fn finish(&mut self) {
        self.nodes
            .sort_unstable_by(|a, b| a.time.total_cmp(&b.time).then(b.seq.cmp(&a.seq)));
    }

    pub fn len(&self) -> usize {
        self.nodes.len()
    }

    pub fn is_empty(&self) -> bool {
        self.nodes.is_empty()
    }

    /// `FindFrequencyMultiplier` @ 0x10002e60. Interpolates pitches and weights,
    /// draws a voice (voice 4 takes the remainder of 100 %), adds the band-limited
    /// randomiser and returns `1.05946309434^semitones`.
    pub fn evaluate(&self, time: f64, rnd: &BreakPointList, rng: &mut MacRandom) -> f64 {
        let (pitch, w) = match self.nodes.first() {
            None => ([0.0; 4], [100i16, 0, 0]),
            Some(head) if time < head.time => (head.pitch, Self::weights(&head.weight)),
            Some(_) => {
                let i = self.nodes.partition_point(|n| n.time < time).saturating_sub(1);
                let p = &self.nodes[i];
                match self.nodes.get(i + 1) {
                    None => (p.pitch, Self::weights(&p.weight)),
                    Some(n) => {
                        let mut dt = n.time - p.time;
                        if dt == 0.0 {
                            dt = 0.01;
                        }
                        let f = (time - p.time) / dt;
                        let mut pitch = [0.0; 4];
                        for k in 0..4 {
                            pitch[k] = f * (n.pitch[k] - p.pitch[k]) + p.pitch[k];
                        }
                        let mut w = [0i16; 3];
                        for k in 0..3 {
                            let dw = trunc16(n.weight[k] - p.weight[k]) as f64;
                            w[k] = trunc16(f * dw + p.weight[k]);
                        }
                        (pitch, w)
                    }
                }
            }
        };

        let roll = trunc16(100.0 * rng.frand());
        let (c1, c2, c3) = (w[0], w[0].wrapping_add(w[1]), w[0].wrapping_add(w[1]).wrapping_add(w[2]));
        let voice = if roll < c1 {
            0
        } else if roll < c2 {
            1
        } else if roll < c3 {
            2
        } else {
            3
        };

        let r = rnd.find_lin(time);
        let semis = pitch[voice] + (r * rng.frand() - 0.5 * r);
        1.059_463_094_34f64.powf(semis)
    }

    /// Interpolated pitches and voice probabilities at `time`, without drawing.
    pub fn voices_at(&self, time: f64) -> [(f32, f32); 4] {
        let (pitch, w) = match self.nodes.first() {
            None => return [(0.0, 1.0), (0.0, 0.0), (0.0, 0.0), (0.0, 0.0)],
            Some(head) if time < head.time => (head.pitch, head.weight),
            Some(_) => {
                let i = self.nodes.partition_point(|n| n.time < time).saturating_sub(1);
                let p = &self.nodes[i];
                match self.nodes.get(i + 1) {
                    None => (p.pitch, p.weight),
                    Some(n) => {
                        let dt = (n.time - p.time).max(0.01);
                        let f = (time - p.time) / dt;
                        let mut pitch = [0.0; 4];
                        let mut w = [0.0; 4];
                        for k in 0..4 {
                            pitch[k] = p.pitch[k] + f * (n.pitch[k] - p.pitch[k]);
                            w[k] = p.weight[k] + f * (n.weight[k] - p.weight[k]);
                        }
                        (pitch, w)
                    }
                }
            }
        };
        let w4 = 100.0 - w[0] - w[1] - w[2];
        let probs = [w[0], w[1], w[2], w4].map(|x| (x / 100.0).clamp(0.0, 1.0) as f32);
        [0, 1, 2, 3].map(|k| (pitch[k] as f32, probs[k]))
    }

    fn weights(w: &[f64; 4]) -> [i16; 3] {
        [trunc16(w[0]), trunc16(w[1]), trunc16(w[2])]
    }
}

/// Score curves sampled for display.
#[derive(Clone, Debug, Default)]
pub struct Lanes {
    /// Granular frequency, grains/s.
    pub gf: Vec<f32>,
    /// Estimated overlapping layers (grains/s × grain length).
    pub layers: Vec<f32>,
    /// Balance 0 (left) .. 2 (right).
    pub balance: Vec<f32>,
    /// Read position in the source, 0..1.
    pub source: Vec<f32>,
    /// Pitch (semitones) and probability (0..1) of the four transposer voices.
    pub voices: Vec<[(f32, f32); 4]>,
}

/// The 23 pentatonic steps copied onto the stack by `PentaScore` (data offset 0xeac).
const PENTATONIC_SCALE: [f32; 23] = [
    -30.0, -28.0, -26.0, -23.0, -21.0, -18.0, -16.0, -14.0, -11.0, -9.0, -6.0, -4.0, -2.0, 1.0,
    3.0, 6.0, 8.0, 10.0, 13.0, 15.0, 18.0, 20.0, 22.0,
];

/// How the two-voice weights of the Classic family are drawn.
#[derive(Clone, Copy)]
enum WeightDraw {
    /// `p = 100 * FRand()` (Hectic, Flowing, Dis).
    Linear,
    /// `p = 100 * FRand() * FRand()`, mirrored to `100 - p` when `(int)p` is odd (Flash, Slack).
    SquaredOddMirror,
}

/// Parameters that distinguish the five Classic-family scores. In the binary these
/// are five copies of the same function with different constants.
struct ClassicSpec {
    loop1: i32,
    loop2: i32,
    weights: WeightDraw,
    voice1_base: f64,
    voice2_scale: f64,
    voice2_offset: f64,
    tail_gf: f64,
    tail_gl: i32,
    tail_gain: f64,
}

/// Complete score: the 16 breakpoint lists the original allocates in `SetFirstBreakPoints`.
pub struct ScoreState {
    pub resolved: ScoreType,
    /// Length the composition plays for (after the length setting).
    pub total_duration: f64,
    /// The score's own length.
    pub native_duration: f64,
    /// True when the composition ends exactly at `total_duration` (Stretch / Cut).
    pub hard_end: bool,
    pub source_duration: f64,

    pub gf: BreakPointList,      // granular frequency (grains/s), exponential
    pub gf_rnd: BreakPointList,  // grain-onset jitter in output samples
    pub gl: BreakPointList,      // grain length (samples)
    pub gl_rnd: BreakPointList,
    pub attack: BreakPointList,  // fraction of the grain
    pub attack_rnd: BreakPointList,
    pub decay: BreakPointList,
    pub decay_rnd: BreakPointList,
    pub gain: BreakPointList,
    pub gain_rnd: BreakPointList,
    pub so: BreakPointList,      // source offset (seconds)
    pub so_rnd: BreakPointList,
    pub balance: BreakPointList, // 0 = left .. 2 = right
    pub balance_rnd: BreakPointList,
    pub fm: FmList,
    pub fm_rnd: BreakPointList,

    seq: u32,
}

impl Default for ScoreState {
    fn default() -> Self {
        Self::new()
    }
}

impl ScoreState {
    /// Allocates every list with enough capacity for the densest score (Flash),
    /// so that `regenerate` never allocates on the audio thread.
    pub fn new() -> Self {
        let cap = 5000;
        let bp = || BreakPointList::with_capacity(cap);
        Self {
            resolved: ScoreType::ClassicFlowing,
            total_duration: 1200.0,
            native_duration: 1200.0,
            hard_end: false,
            source_duration: 1.0,
            gf: bp(),
            gf_rnd: bp(),
            gl: bp(),
            gl_rnd: bp(),
            attack: bp(),
            attack_rnd: bp(),
            decay: bp(),
            decay_rnd: bp(),
            gain: bp(),
            gain_rnd: bp(),
            so: bp(),
            so_rnd: bp(),
            balance: bp(),
            balance_rnd: bp(),
            fm: FmList::with_capacity(cap),
            fm_rnd: bp(),
            seq: 0,
        }
    }

    pub fn generate(score_type: ScoreType, source_duration: f64, seed: u32) -> Self {
        Self::generate_with(score_type, source_duration, seed, Length::default())
    }

    pub fn generate_with(score_type: ScoreType, source_duration: f64, seed: u32, length: Length) -> Self {
        let mut s = Self::new();
        s.regenerate_with(score_type, source_duration, seed, length);
        s
    }

    pub fn regenerate(&mut self, score_type: ScoreType, source_duration: f64, seed: u32) {
        self.regenerate_with(score_type, source_duration, seed, Length::default());
    }

    /// Rebuilds the score in place (no allocation once capacity is reserved),
    /// then applies the length setting.
    pub fn regenerate_with(&mut self, score_type: ScoreType, source_duration: f64, seed: u32, length: Length) {
        let mut rng = MacRandom::new(seed);
        self.clear();
        self.source_duration = source_duration;
        self.set_default_values();

        let resolved = if score_type == ScoreType::Stochastic {
            const ALL: [ScoreType; 11] = [
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
            ];
            ALL[(rng.rand(11) as usize - 1).min(10)]
        } else {
            score_type
        };
        self.resolved = resolved;
        self.total_duration = resolved.duration_secs();

        match resolved {
            ScoreType::ClassicHectic => self.classic(&mut rng, &ClassicSpec {
                loop1: 600,
                loop2: 600,
                weights: WeightDraw::Linear,
                voice1_base: -36.0,
                voice2_scale: 12.0,
                voice2_offset: 12.0,
                tail_gf: 500.0,
                tail_gl: 200,
                tail_gain: 0.03,
            }),
            ScoreType::ClassicFlowing => self.classic(&mut rng, &ClassicSpec {
                loop1: 80,
                loop2: 40,
                weights: WeightDraw::Linear,
                voice1_base: -36.0,
                voice2_scale: 12.0,
                voice2_offset: 12.0,
                tail_gf: 500.0,
                tail_gl: 200,
                tail_gain: 0.03,
            }),
            ScoreType::Flash => self.classic(&mut rng, &ClassicSpec {
                loop1: 2400,
                loop2: 2400,
                weights: WeightDraw::SquaredOddMirror,
                voice1_base: -36.0,
                voice2_scale: 12.0,
                voice2_offset: 0.0,
                tail_gf: 500.0,
                tail_gl: 200,
                tail_gain: 0.03,
            }),
            ScoreType::Slack => self.classic(&mut rng, &ClassicSpec {
                loop1: 40,
                loop2: 20,
                weights: WeightDraw::SquaredOddMirror,
                voice1_base: -24.0,
                voice2_scale: 12.0,
                voice2_offset: 12.0,
                tail_gf: 500.0,
                tail_gl: 200,
                tail_gain: 0.03,
            }),
            ScoreType::Dis => self.classic(&mut rng, &ClassicSpec {
                loop1: 133,
                loop2: 66,
                weights: WeightDraw::Linear,
                voice1_base: -52.0,
                voice2_scale: 20.0,
                voice2_offset: -12.0,
                tail_gf: 1000.0,
                tail_gl: 2000,
                tail_gain: 0.02,
            }),
            ScoreType::Pentathon => self.pentathon(&mut rng),
            ScoreType::Shepard => self.shepard(&mut rng),
            ScoreType::OctavesStretch1Min => self.octaves_stretch(&mut rng, 60.0, 5.0),
            ScoreType::OctavesStretch5Min => self.octaves_stretch(&mut rng, 300.0, 7.0),
            ScoreType::PlainStretch1Min => self.plain_stretch(&mut rng, 60.0),
            ScoreType::PlainStretch5Min => self.plain_stretch(&mut rng, 300.0),
            ScoreType::Stochastic => unreachable!(),
        }

        self.native_duration = self.total_duration;
        self.total_duration = length.effective(self.native_duration);
        self.hard_end = length.mode != LengthMode::Original;
        if length.mode == LengthMode::Stretch {
            self.scale_time(self.total_duration / self.native_duration);
        }
        self.finish();
    }

    /// Multiplies every breakpoint time (except the 1e9 defaults) by `k`.
    fn scale_time(&mut self, k: f64) {
        for l in self.lists_mut() {
            for p in l.points.iter_mut().filter(|p| p.time < DEFAULT_TIME) {
                p.time *= k;
            }
        }
        for n in self.fm.nodes.iter_mut().filter(|n| n.time < DEFAULT_TIME) {
            n.time *= k;
        }
    }

    /// Deterministic curves for display: `n` columns over the composition,
    /// without the randomisers.
    pub fn lanes(&self, n: usize, source_sr: f64) -> Lanes {
        let mut lanes = Lanes::default();
        let dur = self.total_duration.max(1e-3);
        for i in 0..n {
            let t = (i as f64 + 0.5) / n as f64 * dur;
            let gf = self.gf.find_exp(t).abs();
            let gl = self.gl.find_lin(t).max(0.0);
            lanes.gf.push(gf as f32);
            lanes.layers.push((gf * gl / source_sr.max(1.0)) as f32);
            lanes.balance.push(self.balance.find_lin(t).clamp(0.0, 2.0) as f32);
            let so = self.so.find_lin(t) / self.source_duration.max(1e-6);
            lanes.source.push(so.clamp(0.0, 1.0) as f32);
            lanes.voices.push(self.fm.voices_at(t));
        }
        lanes
    }

    /// Evaluates granular frequency at time `t` (`FindGranFreq`, no randomiser).
    pub fn granular_freq(&self, t: f64) -> f64 {
        self.gf.find_exp(t)
    }

    /// `value + rnd * FRand() - 0.5 * rnd`, the shape shared by all `Find*` functions.
    #[inline]
    fn randomised(list: &BreakPointList, rnd: &BreakPointList, t: f64, rng: &mut MacRandom) -> f64 {
        let v = list.find_lin(t);
        let r = rnd.find_lin(t);
        v + (r * rng.frand() - 0.5 * r)
    }

    pub fn grain_length(&self, t: f64, rng: &mut MacRandom) -> i32 {
        Self::randomised(&self.gl, &self.gl_rnd, t, rng) as i32
    }
    pub fn attack(&self, t: f64, rng: &mut MacRandom) -> f64 {
        Self::randomised(&self.attack, &self.attack_rnd, t, rng)
    }
    pub fn decay(&self, t: f64, rng: &mut MacRandom) -> f64 {
        Self::randomised(&self.decay, &self.decay_rnd, t, rng)
    }
    pub fn gain(&self, t: f64, rng: &mut MacRandom) -> f64 {
        Self::randomised(&self.gain, &self.gain_rnd, t, rng)
    }
    pub fn source_offset(&self, t: f64, rng: &mut MacRandom) -> f64 {
        Self::randomised(&self.so, &self.so_rnd, t, rng)
    }
    pub fn balance(&self, t: f64, rng: &mut MacRandom) -> f64 {
        Self::randomised(&self.balance, &self.balance_rnd, t, rng)
    }
    pub fn freq_multiplier(&self, t: f64, rng: &mut MacRandom) -> f64 {
        self.fm.evaluate(t, &self.fm_rnd, rng)
    }

    // --- Set* helpers, argument order as in the binary: (value, time) ---

    fn set_gf(&mut self, v: f64, t: f64) {
        self.gf.set(v, t, &mut self.seq);
    }
    fn gf_rnd(&mut self, v: f64, t: f64) {
        self.gf_rnd.set(v, t, &mut self.seq);
    }
    fn set_gl(&mut self, t: f64, v: i32) {
        self.gl.set(v as f64, t, &mut self.seq);
    }
    fn gl_rnd(&mut self, v: f64, t: f64) {
        self.gl_rnd.set(v, t, &mut self.seq);
    }
    fn set_a(&mut self, v: f64, t: f64) {
        self.attack.set(v, t, &mut self.seq);
    }
    fn a_rnd(&mut self, v: f64, t: f64) {
        self.attack_rnd.set(v, t, &mut self.seq);
    }
    fn set_d(&mut self, v: f64, t: f64) {
        self.decay.set(v, t, &mut self.seq);
    }
    fn d_rnd(&mut self, v: f64, t: f64) {
        self.decay_rnd.set(v, t, &mut self.seq);
    }
    fn set_g(&mut self, v: f64, t: f64) {
        self.gain.set(v, t, &mut self.seq);
    }
    fn g_rnd(&mut self, v: f64, t: f64) {
        self.gain_rnd.set(v, t, &mut self.seq);
    }
    fn set_so(&mut self, v: f64, t: f64) {
        self.so.set(v, t, &mut self.seq);
    }
    fn so_rnd(&mut self, v: f64, t: f64) {
        self.so_rnd.set(v, t, &mut self.seq);
    }
    /// `SetB` stores `1.0 + value`: the scores think in -1..1, the engine in 0..2.
    fn set_b(&mut self, v: f64, t: f64) {
        self.balance.set(1.0 + v, t, &mut self.seq);
    }
    fn b_rnd(&mut self, v: f64, t: f64) {
        self.balance_rnd.set(v, t, &mut self.seq);
    }
    fn set_fm(&mut self, pitch: [f64; 4], t: f64, weight: [i32; 4]) {
        self.fm.set(pitch, t, weight, &mut self.seq);
    }
    fn fm_rnd(&mut self, v: f64, t: f64) {
        self.fm_rnd.set(v, t, &mut self.seq);
    }

    /// `SetDefaultValues` @ 0x10005524: one breakpoint per list at t = 1e9.
    fn set_default_values(&mut self) {
        let t = DEFAULT_TIME;
        self.set_gf(500.0, t);
        self.gf_rnd(0.0, t);
        self.set_gl(t, 500);
        self.gl_rnd(0.0, t);
        self.set_a(0.01, t);
        self.a_rnd(0.0, t);
        self.set_d(0.01, t);
        self.d_rnd(0.0, t);
        self.set_g(1.0, t);
        self.g_rnd(0.0, t);
        self.set_fm([0.0; 4], t, [100, 0, 0, 0]);
        self.fm_rnd(0.0, t);
        self.set_so(0.0, t);
        self.so_rnd(0.0, t);
        self.set_b(0.0, t);
        self.b_rnd(0.0, t);
    }

    // --- Score generators ---

    /// `ClassicScoreHectic` @ 0x10006f30 and its siblings `ClassicScoreFlowing`,
    /// `Flash`, `Slack` and `Dis`: the same two loops with different constants.
    fn classic(&mut self, rng: &mut MacRandom, spec: &ClassicSpec) {
        let dur = 1200.0;
        let src = self.source_duration;

        for _ in 0..=spec.loop1 {
            for _ in 0..2 {
                let x = trunc16(75.0 * rng.frand()) as i32;
                let t = dur * rng.frand();
                self.set_gf((x * x) as f64, t);
            }
            let t = rng.frand();
            let v = rng.frand();
            self.gf_rnd(500.0 * v, dur * t);
            let t = rng.frand();
            let v = rng.frand();
            self.set_gl(dur * t, (4000.0 * v) as i32);
            let t = rng.frand();
            let v = rng.frand();
            self.gl_rnd(500.0 * v, dur * t);
            let t = rng.frand();
            let v = rng.frand();
            self.set_b(3.0 * v - 1.5, dur * t);
            let t = rng.frand();
            let v = rng.frand();
            self.b_rnd(6.0 * v, dur * t);
        }

        for _ in 0..=spec.loop2 {
            let k = trunc16(20.0 * rng.frand() + 0.001) as i32;
            let t = rng.frand();
            let v = rng.frand();
            self.set_so((src - 0.05) * v, dur * t);
            let t = rng.frand();
            let v = rng.frand();
            self.so_rnd((k * k) as f64 * v / 1000.0, dur * t);

            let mut p = (100.0 * rng.frand()) as f32;
            if let WeightDraw::SquaredOddMirror = spec.weights {
                p = (p as f64 * rng.frand()) as f32;
                if (p as i32) & 1 != 0 {
                    p = (100.0 - p as f64) as f32;
                }
            }
            let t = rng.frand();
            let v2 = rng.frand();
            let v1 = rng.frand();
            self.set_fm(
                [
                    18.0 * v1 + spec.voice1_base,
                    spec.voice2_scale * v2 + spec.voice2_offset,
                    0.0,
                    0.0,
                ],
                dur * t,
                [p as i32, (100.0f32 - p) as i32, 0, 0],
            );
            let t = rng.frand();
            let v = rng.frand();
            self.fm_rnd(3.0 * v, dur * t);
        }

        self.set_gf(spec.tail_gf, 0.0);
        self.set_so(0.1, 0.0);
        self.set_gl(0.0, spec.tail_gl);
        self.set_a(0.1, 0.0);
        self.a_rnd(0.0, 0.0);
        self.set_d(0.1, 0.0);
        self.d_rnd(0.0, 0.0);
        self.set_g(spec.tail_gain, 15.0);
        self.g_rnd(0.0, 0.0);
    }

    /// `PentaScore` @ 0x10008a78.
    fn pentathon(&mut self, rng: &mut MacRandom) {
        let dur = 1200.0;
        self.set_gf(2000.0, 0.0);
        self.gf_rnd(100.0, 0.0);
        self.set_gl(0.0, 2000);
        self.gl_rnd(0.0, 0.0);
        self.set_a(0.2, 0.0);
        self.a_rnd(0.0, 0.0);
        self.set_d(0.2, 0.0);
        self.d_rnd(0.0, 0.0);
        self.set_g(0.05, 0.0);
        self.g_rnd(0.0, 0.0);

        let steps = (dur * (1.0 / 8.0)) as i32;
        let note = |i: i32| PENTATONIC_SCALE[i.clamp(0, 22) as usize] as f64;
        let mut upper = rng.rand(10) + 5;
        let mut lower = rng.rand(5) + 3;
        let mut countdown = 4;
        let mut t = 0.0f32;

        for _ in 0..=steps {
            t = (2.5 + (16.0 * rng.frand() + t as f64)) as f32;
            self.set_fm([note(upper), note(lower), 0.0, 0.0], t as f64, [50, 50, 0, 0]);

            upper += rng.rand(5) - 3;
            if !(6..=19).contains(&upper) {
                upper = rng.rand(15) + 5;
            }
            if countdown == 0 {
                countdown = 2;
                lower += rng.rand(5) - 3;
                if !(0..=12).contains(&lower) {
                    lower = rng.rand(10);
                }
                let at = dur * rng.frand();
                let v = self.source_duration * rng.frand();
                self.set_so(v, at);
            } else {
                countdown -= 1;
            }
            self.set_fm(
                [note(upper), note(lower), 0.0, 0.0],
                (t + 2.5f32) as f64,
                [50, 50, 0, 0],
            );
        }

        self.so_rnd(0.1, 0.0);
        self.fm_rnd(0.0, 0.0);
        self.set_b(0.0, 0.0);
        self.b_rnd(6.0, 0.0);
    }

    /// `Shepard` @ 0x1000842c: four voices that fade in high, glide down while
    /// audible and jump back up while silent (weight 0), slowing down every cycle.
    fn shepard(&mut self, rng: &mut MacRandom) {
        let dur = 1200.0;
        self.set_gf(2000.0, 0.0);
        self.gf_rnd(100.0, 0.0);
        self.set_gl(0.0, 2000);
        self.gl_rnd(0.0, 0.0);
        self.set_a(0.1, 0.0);
        self.a_rnd(0.0, 0.0);
        self.set_d(0.1, 0.0);
        self.d_rnd(0.0, 0.0);
        self.set_g(0.1, 0.0);
        self.g_rnd(0.0, 0.0);

        let w25 = [25, 25, 25, 25];
        self.set_fm([24.0, 25.0, 26.0, 24.0], 0.0, w25);
        self.set_fm([0.0, 25.0, 26.0, 24.0], 5.0, w25);
        self.set_fm([-24.0, 0.0, 26.0, 24.0], 10.0, w25);

        let mut low = -28.0f32;
        let mut high = 18.0f32;
        let mid = -4.0f32;
        let mut speed = 1.3f32;
        let mut t = 15.0f32;
        let step = |rng: &mut MacRandom, t: f32, speed: f32| speed * (rng.rand(3) + 3) as f32 + t;
        let p = |x: f32| x as f64;

        while (t as f64) < dur {
            let r24 = rng.rand(8) as f32;
            let r23 = rng.rand(8) as f32;
            let r22 = rng.rand(8) as f32;
            let r21 = rng.rand(8) as f32;
            let r20 = rng.rand(6) as f32;
            let r19 = rng.rand(6) as f32;
            let r18 = rng.rand(6) as f32;
            let r17 = rng.rand(6) as f32;

            let wa = [0, 30, 30, 40];
            self.set_fm([p(low + r24), p(low + r23), p(mid), p(high + r17)], p(t), wa);
            let v20 = high + r20;
            self.set_fm([p(5.0 + v20), p(low + r23), p(mid), p(high + r17)], 0.1 + p(t), wa);

            t = step(rng, t, speed);
            let wb = [30, 0, 30, 40];
            self.set_fm([p(v20), p(low + r23), p(low + r22), p(mid)], p(t), wb);
            let v19 = high + r19;
            self.set_fm([p(v20), p(5.0 + v19), p(low + r22), p(mid)], 0.1 + p(t), wb);

            t = step(rng, t, speed);
            let wc = [30, 30, 0, 40];
            self.set_fm([p(mid), p(v19), p(low + r22), p(low + r21)], p(t), wc);
            let v18 = high + r18;
            self.set_fm([p(mid), p(v19), p(5.0 + v18), p(low + r21)], 0.1 + p(t), wc);

            t = step(rng, t, speed);
            let wd = [35, 35, 30, 0];
            self.set_fm([p(low + r24), p(mid), p(v18), p(low + r21)], p(t), wd);
            self.set_fm(
                [p(low + r24), p(mid), p(v18), p(5.0 + (high + r17))],
                0.1 + p(t),
                wd,
            );

            t = step(rng, t, speed);
            low = (low as f64 - 1.5) as f32;
            high = (high as f64 - 1.5) as f32;
            speed = (0.3 + speed as f64) as f32;
        }

        self.fm_rnd(1.0, 0.0);
        self.set_b(0.0, 0.0);
        self.b_rnd(8.0, 0.0);
        self.set_so(0.2, 0.0);
        self.set_so(self.source_duration - 0.2, dur);
        self.so_rnd(0.1, 0.0);
    }

    /// `stretch1Score` @ 0x10008ddc / `stretch5Score` @ 0x10009068.
    fn octaves_stretch(&mut self, rng: &mut MacRandom, dur: f64, b_rnd: f64) {
        self.set_gf(3000.0, 0.0);
        self.gf_rnd(100.0, 0.0);
        self.set_gl(0.0, 3000);
        self.gl_rnd(100.0, 0.0);
        self.set_a(0.05, 0.0);
        self.a_rnd(0.0, 0.0);
        self.set_d(0.05, 0.0);
        self.d_rnd(0.0, 0.0);
        self.set_g(0.05, 0.0);
        self.g_rnd(0.0, 0.0);

        let nodes = (dur as f32 * 0.125) as i32;
        for _ in 0..nodes {
            let mut w = rng.rand(30);
            if w & 1 != 0 {
                w = 60 - w;
            }
            let t = dur * rng.frand();
            self.set_fm(
                [0.0, -24.0, 12.0, 0.0],
                t,
                [40, w, (50.0f32 - w as f32) as i32, 0],
            );
            self.fm_rnd(0.0, 0.0);
        }

        self.set_b(0.0, 0.0);
        self.b_rnd(b_rnd, 0.0);
        self.set_so(0.0, 0.0);
        self.set_so(self.source_duration, dur);
        self.so_rnd(0.0, 0.0);
    }

    /// `plainStretch1` @ 0x100092f0 / `plainStretch5` @ 0x10009494.
    fn plain_stretch(&mut self, rng: &mut MacRandom, dur: f64) {
        self.set_gf(2500.0, 0.0);
        self.gf_rnd(100.0, 0.0);
        self.set_gl(0.0, 2000);
        self.gl_rnd(100.0, 0.0);
        self.set_a(0.05, 0.0);
        self.a_rnd(0.0, 0.0);
        self.set_d(0.05, 0.0);
        self.d_rnd(0.0, 0.0);
        self.set_g(0.07, 0.0);
        self.g_rnd(0.0, 0.0);
        let t = dur * rng.frand();
        self.set_fm([0.0; 4], t, [100, 0, 0, 0]);
        self.fm_rnd(0.0, 0.0);
        self.set_b(0.0, 0.0);
        self.b_rnd(7.0, 0.0);
        self.set_so(0.0, 0.0);
        self.set_so(self.source_duration, dur);
        self.so_rnd(0.0, 0.0);
    }

    fn lists_mut(&mut self) -> [&mut BreakPointList; 15] {
        [
            &mut self.gf,
            &mut self.gf_rnd,
            &mut self.gl,
            &mut self.gl_rnd,
            &mut self.attack,
            &mut self.attack_rnd,
            &mut self.decay,
            &mut self.decay_rnd,
            &mut self.gain,
            &mut self.gain_rnd,
            &mut self.so,
            &mut self.so_rnd,
            &mut self.balance,
            &mut self.balance_rnd,
            &mut self.fm_rnd,
        ]
    }

    fn clear(&mut self) {
        self.seq = 0;
        for l in self.lists_mut() {
            l.points.clear();
        }
        self.fm.nodes.clear();
    }

    fn finish(&mut self) {
        for l in self.lists_mut() {
            l.finish();
        }
        self.fm.finish();
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn make(t: ScoreType) -> ScoreState {
        ScoreState::generate(t, 4.0, 4242)
    }

    #[test]
    fn classic_family_point_counts_match_loop_bounds() {
        // Two SetGF per loop-1 iteration (inclusive bound), plus the tail and default.
        for (t, n1, n2) in [
            (ScoreType::ClassicHectic, 600, 600),
            (ScoreType::ClassicFlowing, 80, 40),
            (ScoreType::Flash, 2400, 2400),
            (ScoreType::Slack, 40, 20),
            (ScoreType::Dis, 133, 66),
        ] {
            let s = make(t);
            assert_eq!(s.gf.len(), 2 * (n1 + 1) + 2, "{t:?} GF");
            assert_eq!(s.gl.len(), (n1 + 1) + 2, "{t:?} GL");
            assert_eq!(s.fm.len(), (n2 + 1) + 1, "{t:?} FM");
            assert_eq!(s.so.len(), (n2 + 1) + 2, "{t:?} SO");
        }
    }

    #[test]
    fn classic_values_stay_in_decoded_ranges() {
        let s = make(ScoreType::ClassicHectic);
        assert!(s.gf.points.iter().all(|p| p.value <= 74.0 * 74.0 || p.time >= DEFAULT_TIME));
        assert!(s.gl.points.iter().all(|p| p.value < 4000.0 || p.time >= DEFAULT_TIME));
        assert!(s.balance.points.iter().all(|p| (-0.5..=2.5).contains(&p.value)));
        for n in s.fm.nodes.iter().filter(|n| n.time < DEFAULT_TIME) {
            assert!((-36.0..=-18.0).contains(&n.pitch[0]));
            assert!((12.0..=24.0).contains(&n.pitch[1]));
            assert!(n.weight[0] + n.weight[1] >= 99.0);
        }
        assert!((s.gain.find_lin(600.0) - 0.03).abs() < 1e-5); // drifts toward the 1e9 default, as in the original
        let d = make(ScoreType::Dis);
        for n in d.fm.nodes.iter().filter(|n| n.time < DEFAULT_TIME) {
            assert!((-52.0..=-34.0).contains(&n.pitch[0]));
            assert!((-12.0..=8.0).contains(&n.pitch[1]));
        }
    }

    #[test]
    fn shepard_descends_and_slows_down() {
        let s = make(ScoreType::Shepard);
        let early: Vec<_> = s.fm.nodes.iter().filter(|n| n.time > 15.0 && n.time < 60.0).collect();
        let late: Vec<_> = s.fm.nodes.iter().filter(|n| n.time > 900.0 && n.time < 1200.0).collect();
        let min_pitch = |v: &[&FmNode]| v.iter().map(|n| n.pitch[0].min(n.pitch[1])).fold(f64::MAX, f64::min);
        assert!(min_pitch(&late) < min_pitch(&early) - 10.0);
        // Node spacing grows (speed 1.3 s -> +0.3 s per cycle).
        assert!(late.len() < early.len() * 300 / 45);
        assert!((s.gain.find_lin(10.0) - 0.1).abs() < 1e-6);
    }

    #[test]
    fn pentathon_uses_scale_notes_and_two_registers() {
        let s = make(ScoreType::Pentathon);
        for n in s.fm.nodes.iter().filter(|n| n.time < DEFAULT_TIME) {
            assert!(PENTATONIC_SCALE.contains(&(n.pitch[0] as f32)));
            assert!(PENTATONIC_SCALE.contains(&(n.pitch[1] as f32)));
            assert_eq!(n.weight[0], 50.0);
        }
        assert_eq!(s.fm.len(), 2 * 151 + 1);
    }

    #[test]
    fn octave_stretch_weights_sum_to_ninety() {
        let s = make(ScoreType::OctavesStretch1Min);
        let nodes: Vec<_> = s.fm.nodes.iter().filter(|n| n.time < DEFAULT_TIME).collect();
        assert_eq!(nodes.len(), 7);
        for n in nodes {
            assert_eq!(n.pitch, [0.0, -24.0, 12.0, 0.0]);
            assert_eq!(n.weight[0] + n.weight[1] + n.weight[2], 90.0);
        }
        assert_eq!(s.so.find_lin(60.0), 4.0);
    }

    #[test]
    fn stretch_scales_all_breakpoint_times() {
        let len = Length { mode: LengthMode::Stretch, secs: 120.0 };
        let a = ScoreState::generate(ScoreType::ClassicHectic, 4.0, 7);
        let b = ScoreState::generate_with(ScoreType::ClassicHectic, 4.0, 7, len);
        assert_eq!(b.total_duration, 120.0);
        assert!(b.hard_end);
        // Same composition, ten times faster.
        assert!((a.gf.find_exp(600.0) - b.gf.find_exp(60.0)).abs() < 1e-6);
        assert!((a.gain.find_lin(15.0) - b.gain.find_lin(1.5)).abs() < 1e-6);
        let cut = ScoreState::generate_with(
            ScoreType::OctavesStretch1Min,
            4.0,
            7,
            Length { mode: LengthMode::Cut, secs: 600.0 },
        );
        assert_eq!(cut.total_duration, 60.0);
    }

    #[test]
    fn lanes_have_requested_columns() {
        let s = make(ScoreType::Shepard);
        let l = s.lanes(300, 44100.0);
        assert_eq!(l.gf.len(), 300);
        assert_eq!(l.voices.len(), 300);
        assert!(l.voices.iter().all(|v| (v.iter().map(|x| x.1).sum::<f32>() - 1.0).abs() < 0.02));
    }

    #[test]
    fn regenerate_does_not_grow_capacity() {
        let mut s = ScoreState::new();
        let cap = s.gf.points.capacity();
        s.regenerate(ScoreType::Flash, 3.0, 1);
        assert_eq!(s.gf.points.capacity(), cap);
    }

    #[test]
    fn mersenne_reduction_equals_modulo() {
        let mut r = MacRandom::new(0x1234_5678);
        let mut seed = 0x1234_5678u64;
        for _ in 0..1_000_000 {
            seed = seed * 16807 % 0x7fff_ffff;
            r.random();
            assert_eq!(r.seed as u64, seed);
        }
    }

    #[test]
    fn mac_random_matches_park_miller() {
        let mut r = MacRandom::new(1);
        assert_eq!(r.random(), 16807);
        assert_eq!(r.seed, 16807);
        r.random();
        assert_eq!(r.seed, 282_475_249);
    }
}

#[cfg(test)]
mod bench {
    use super::*;
    #[test]
    #[ignore]
    fn gen_cost() {
        let mut s = ScoreState::new();
        for t in [ScoreType::Flash, ScoreType::ClassicHectic] {
            let t0 = std::time::Instant::now();
            for i in 0..50 {
                s.regenerate(t, 4.0, i);
            }
            let mut r = MacRandom::new(3);
            let t1 = std::time::Instant::now();
            let mut acc = 0.0;
            for _ in 0..100_000 {
                acc += r.frand();
            }
            println!(
                "{:?}: {:.0} us/regenerate, frand {:.1} ns ({acc:.0})",
                t,
                t0.elapsed().as_secs_f64() * 1e6 / 50.0,
                t1.elapsed().as_secs_f64() * 1e9 / 100_000.0
            );
        }
    }
}
