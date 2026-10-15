//! Score generators and breakpoint parameter interpolation.
//! Faithfully reverse-engineered from Th0nk 0+2 (Audio Ease, 1995-1997).

use truce::prelude::*;

/// All 12 Th0nk composition methods (scores).
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
    pub fn description(&self) -> &'static str {
        match self {
            ScoreType::ClassicFlowing => {
                "'Classic Flowing', from the original Thonk, provides slowly evolving natural soundscapes."
            }
            ScoreType::ClassicHectic => {
                "'Classic Hectic', from the original Thonk, moves around fast. You can clip short events from the output for FX samples. Give it hot leveled audio."
            }
            ScoreType::Pentathon => {
                "'Pentathon' gives a two voice, brownian walk, pentatonic counterpoint. Feed it one note or a very short melody for solid tonal results."
            }
            ScoreType::Shepard => {
                "Roger 'Shepard' invented circular pitch. This stream keeps ascending or descending forever, creating auditory illusions."
            }
            ScoreType::Dis => {
                "'Dis' gives dark subterranean textures while moving a lot. Shifts pitches down 3 to 4 octaves into deep seismic rumble."
            }
            ScoreType::OctavesStretch1Min => {
                "This 'octaves stretch' fits the input to 1 minute, while octaves fade in and out. Direction and tonality of the input are preserved."
            }
            ScoreType::OctavesStretch5Min => {
                "This 'octaves stretch' fits the input to 5 minutes, while octaves fade in and out. Direction and tonality of the input are preserved."
            }
            ScoreType::PlainStretch1Min => {
                "Plain stretch fits the input audio to exactly 1 minute without pitch alterations."
            }
            ScoreType::PlainStretch5Min => {
                "Plain stretch fits the input audio to exactly 5 minutes without pitch alterations."
            }
            ScoreType::Flash => {
                "Classic Hectic, Flash really sprints through your input (2400 points). And it likes its breakfast hot and dense."
            }
            ScoreType::Slack => {
                "Slack is not the hand of God. Slack is the way the wind swirls, and the dust settles, eons after God has passed by. (Kurt Vonnegut)"
            }
            ScoreType::Stochastic => {
                "ztochazt!k = akadem!k wank: Chooses randomly among all available scores on every invocation."
            }
        }
    }

    /// Default rendering duration in seconds for this score method.
    pub fn duration_secs(&self) -> f64 {
        match self {
            ScoreType::OctavesStretch1Min | ScoreType::PlainStretch1Min => 60.0,
            ScoreType::OctavesStretch5Min | ScoreType::PlainStretch5Min => 300.0,
            _ => 1200.0, // 20 minutes (standard Th0nk score duration)
        }
    }
}

/// Linear congruential random number generator emulating the classic Mac OS Toolbox `Random()`.
pub struct FastRand {
    seed: u32,
}

impl FastRand {
    pub fn new(seed: u32) -> Self {
        Self {
            seed: if seed == 0 { 0x12345678 } else { seed },
        }
    }

    /// Emulates Mac OS Toolbox Random() returning float in [0.0, 1.0].
    pub fn frand(&mut self) -> f64 {
        // Linear congruential generator: next = (seed * 1664525 + 1013904223)
        self.seed = self.seed.wrapping_mul(1664525).wrapping_add(1013904223);
        (self.seed as f64) / 4294967296.0
    }

    pub fn rand_int(&mut self, max: i32) -> i32 {
        if max <= 0 {
            return 0;
        }
        (self.frand() * (max as f64)) as i32
    }
}

/// A breakpoint in a parameter timeline.
#[derive(Clone, Copy, Debug)]
pub struct BreakPoint {
    pub time: f64,
    pub value: f64,
}

/// Breakpoint timeline list.
#[derive(Clone, Default, Debug)]
pub struct BreakPointList {
    points: Vec<BreakPoint>,
}

impl BreakPointList {
    pub fn new() -> Self {
        Self { points: Vec::new() }
    }

    pub fn insert(&mut self, time: f64, value: f64) {
        let bp = BreakPoint { time, value };
        let idx = self
            .points
            .binary_search_by(|p| p.time.partial_cmp(&time).unwrap())
            .unwrap_or_else(|i| i);
        self.points.insert(idx, bp);
    }

    /// Linear parameter interpolation (FindLinParm).
    pub fn find_lin(&self, time: f64) -> f64 {
        if self.points.is_empty() {
            return 0.0;
        }
        if time <= self.points[0].time {
            return self.points[0].value;
        }
        if time >= self.points.last().unwrap().time {
            return self.points.last().unwrap().value;
        }

        // Binary search
        let idx = match self
            .points
            .binary_search_by(|p| p.time.partial_cmp(&time).unwrap())
        {
            Ok(i) => return self.points[i].value,
            Err(i) => i,
        };

        let p0 = &self.points[idx - 1];
        let p1 = &self.points[idx];
        let dt = (p1.time - p0.time).max(1e-5);
        let frac = (time - p0.time) / dt;
        p0.value + frac * (p1.value - p0.value)
    }

    /// Exponential parameter interpolation (FindExpParm).
    pub fn find_exp(&self, time: f64) -> f64 {
        if self.points.is_empty() {
            return 1.0;
        }
        if time <= self.points[0].time {
            return self.points[0].value.max(1e-5);
        }
        if time >= self.points.last().unwrap().time {
            return self.points.last().unwrap().value.max(1e-5);
        }

        let idx = match self
            .points
            .binary_search_by(|p| p.time.partial_cmp(&time).unwrap())
        {
            Ok(i) => return self.points[i].value,
            Err(i) => i,
        };

        let p0 = &self.points[idx - 1];
        let p1 = &self.points[idx];
        let dt = (p1.time - p0.time).max(1e-5);
        let frac = (time - p0.time) / dt;

        let v0 = p0.value.max(1e-5);
        let v1 = p1.value.max(1e-5);
        (v0.ln() + frac * (v1.ln() - v0.ln())).exp()
    }
}

/// A 4-voice statistical pitch transposition node.
#[derive(Clone, Copy, Debug)]
pub struct FMNode {
    pub time: f64,
    pub pitches: [f64; 4],      // Voice 1, 2, 3, 4 pitch transpositions in semitones
    pub probabilities: [f64; 4], // Cumulative or weights
}

/// 4-voice statistical transposition timeline.
#[derive(Clone, Default, Debug)]
pub struct FMList {
    nodes: Vec<FMNode>,
}

impl FMList {
    pub fn new() -> Self {
        Self { nodes: Vec::new() }
    }

    pub fn insert(&mut self, time: f64, pitches: [f64; 4], weights: [f64; 4]) {
        let node = FMNode {
            time,
            pitches,
            probabilities: weights,
        };
        let idx = self
            .nodes
            .binary_search_by(|p| p.time.partial_cmp(&time).unwrap())
            .unwrap_or_else(|i| i);
        self.nodes.insert(idx, node);
    }

    /// Evaluates the statistical 4-voice transposition multiplier at `time`.
    pub fn evaluate(
        &self,
        time: f64,
        rnd_list: &BreakPointList,
        rng: &mut FastRand,
    ) -> f64 {
        if self.nodes.is_empty() {
            return 1.0;
        }

        let (pitches, probs) = if time <= self.nodes[0].time {
            (self.nodes[0].pitches, self.nodes[0].probabilities)
        } else if time >= self.nodes.last().unwrap().time {
            let last = self.nodes.last().unwrap();
            (last.pitches, last.probabilities)
        } else {
            let idx = match self
                .nodes
                .binary_search_by(|p| p.time.partial_cmp(&time).unwrap())
            {
                Ok(i) => {
                    let n = &self.nodes[i];
                    return Self::sample_voice(&n.pitches, &n.probabilities, time, rnd_list, rng);
                }
                Err(i) => i,
            };

            let n0 = &self.nodes[idx - 1];
            let n1 = &self.nodes[idx];
            let dt = (n1.time - n0.time).max(1e-5);
            let frac = (time - n0.time) / dt;

            let mut p = [0.0; 4];
            let mut pr = [0.0; 4];
            for k in 0..4 {
                p[k] = n0.pitches[k] + frac * (n1.pitches[k] - n0.pitches[k]);
                pr[k] = n0.probabilities[k] + frac * (n1.probabilities[k] - n0.probabilities[k]);
            }
            (p, pr)
        };

        Self::sample_voice(&pitches, &probs, time, rnd_list, rng)
    }

    fn sample_voice(
        pitches: &[f64; 4],
        probs: &[f64; 4],
        time: f64,
        rnd_list: &BreakPointList,
        rng: &mut FastRand,
    ) -> f64 {
        let total_weight: f64 = probs.iter().sum();
        let roll = rng.frand() * total_weight.max(1.0);

        let mut cum = 0.0;
        let mut chosen_pitch = pitches[0];
        for k in 0..4 {
            cum += probs[k];
            if roll <= cum {
                chosen_pitch = pitches[k];
                break;
            }
        }

        // Add band-limited random jitter: rnd * (FRand() - 0.5)
        let rnd = rnd_list.find_lin(time);
        let jitter = rnd * (rng.frand() - 0.5);
        let final_semitones = chosen_pitch + jitter;

        // 2^(semitones / 12)
        2.0f64.powf(final_semitones / 12.0)
    }
}

/// Pentatonic scale steps table reverse-engineered from offset 0xeac of Th0nk.
const PENTATONIC_SCALE: [f64; 23] = [
    -30.0, -28.0, -26.0, -23.0, -21.0, -18.0, -16.0, -14.0, -11.0, -9.0, -6.0, -4.0, -2.0, 1.0,
    3.0, 6.0, 8.0, 10.0, 13.0, 15.0, 18.0, 20.0, 22.0,
];

/// Complete Score parameter state containing all interpolated lists.
pub struct ScoreState {
    pub total_duration: f64,
    pub source_duration: f64,

    pub gf: BreakPointList,    // Granular Frequency
    pub gf_rnd: BreakPointList,
    pub gl: BreakPointList,    // Grain Length (samples)
    pub gl_rnd: BreakPointList,
    pub attack: BreakPointList, // Attack fraction [0.0, 1.0]
    pub attack_rnd: BreakPointList,
    pub decay: BreakPointList,  // Decay fraction [0.0, 1.0]
    pub decay_rnd: BreakPointList,
    pub gain: BreakPointList,   // Gain multiplier
    pub gain_rnd: BreakPointList,
    pub so: BreakPointList,     // Source Offset (seconds)
    pub so_rnd: BreakPointList,
    pub balance: BreakPointList, // Stereo Balance [0.0 .. 2.0], centered around 1.0
    pub balance_rnd: BreakPointList,
    pub time_jitter: BreakPointList, // Jitter for grain placement
    pub fm: FMList,             // 4-voice statistical pitch transposer
    pub fm_rnd: BreakPointList,
}

impl ScoreState {
    /// Generates a score for given method and input source duration.
    pub fn generate(
        score_type: ScoreType,
        source_duration: f64,
        seed: u32,
    ) -> Self {
        let mut rng = FastRand::new(seed);

        let resolved_score = if score_type == ScoreType::Stochastic {
            let scores = [
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
            scores[rng.rand_int(scores.len() as i32) as usize]
        } else {
            score_type
        };

        match resolved_score {
            ScoreType::ClassicHectic => Self::gen_hectic(source_duration, &mut rng),
            ScoreType::ClassicFlowing => Self::gen_flowing(source_duration, &mut rng),
            ScoreType::Pentathon => Self::gen_pentathon(source_duration, &mut rng),
            ScoreType::Shepard => Self::gen_shepard(source_duration, &mut rng),
            ScoreType::Dis => Self::gen_dis(source_duration, &mut rng),
            ScoreType::OctavesStretch1Min => Self::gen_stretch(source_duration, 60.0, true, &mut rng),
            ScoreType::OctavesStretch5Min => Self::gen_stretch(source_duration, 300.0, true, &mut rng),
            ScoreType::PlainStretch1Min => Self::gen_stretch(source_duration, 60.0, false, &mut rng),
            ScoreType::PlainStretch5Min => Self::gen_stretch(source_duration, 300.0, false, &mut rng),
            ScoreType::Flash => Self::gen_flash(source_duration, &mut rng),
            ScoreType::Slack => Self::gen_slack(source_duration, &mut rng),
            ScoreType::Stochastic => unreachable!(),
        }
    }

    /// Evaluates granular frequency at time `t`.
    pub fn get_granular_freq(&self, t: f64, rng: &mut FastRand) -> f64 {
        let base = self.gf.find_exp(t);
        let rnd = self.gf_rnd.find_lin(t);
        (base + rnd * (rng.frand() - 0.5)).max(1.0)
    }

    /// Evaluates grain length in samples at time `t`.
    pub fn get_grain_length(&self, t: f64, rng: &mut FastRand) -> usize {
        let base = self.gl.find_lin(t);
        let rnd = self.gl_rnd.find_lin(t);
        (base + rnd * (rng.frand() - 0.5)).max(64.0) as usize
    }

    /// Evaluates attack fraction in [0.01, 0.5].
    pub fn get_attack(&self, t: f64, rng: &mut FastRand) -> f64 {
        let base = self.attack.find_lin(t);
        let rnd = self.attack_rnd.find_lin(t);
        (base + rnd * (rng.frand() - 0.5)).clamp(0.001, 0.5)
    }

    /// Evaluates decay fraction in [0.01, 0.5].
    pub fn get_decay(&self, t: f64, rng: &mut FastRand) -> f64 {
        let base = self.decay.find_lin(t);
        let rnd = self.decay_rnd.find_lin(t);
        (base + rnd * (rng.frand() - 0.5)).clamp(0.001, 0.5)
    }

    /// Evaluates grain gain multiplier.
    pub fn get_gain(&self, t: f64, rng: &mut FastRand) -> f64 {
        let base = self.gain.find_lin(t);
        let rnd = self.gain_rnd.find_lin(t);
        (base + rnd * (rng.frand() - 0.5)).max(0.0)
    }

    /// Evaluates source position in seconds.
    pub fn get_source_offset(&self, t: f64, rng: &mut FastRand) -> f64 {
        let base = self.so.find_lin(t);
        let rnd = self.so_rnd.find_lin(t);
        (base + rnd * (rng.frand() - 0.5)).clamp(0.0, self.source_duration.max(0.1))
    }

    /// Evaluates stereo balance in [0.0, 2.0] (centered at 1.0).
    pub fn get_balance(&self, t: f64, rng: &mut FastRand) -> f64 {
        let base = self.balance.find_lin(t);
        let rnd = self.balance_rnd.find_lin(t);
        (base + rnd * (rng.frand() - 0.5)).clamp(0.0, 2.0)
    }

    /// Evaluates pitch transposition multiplier ($2^{\text{semitones}/12}$).
    pub fn get_freq_multiplier(&self, t: f64, rng: &mut FastRand) -> f64 {
        self.fm.evaluate(t, &self.fm_rnd, rng)
    }

    /// Evaluates grain placement time jitter.
    pub fn get_time_jitter(&self, t: f64, rng: &mut FastRand) -> f64 {
        let jitter_range = self.time_jitter.find_lin(t);
        jitter_range * (rng.frand() - 0.5)
    }

    // --- Score Generators ---

    fn gen_hectic(source_duration: f64, rng: &mut FastRand) -> Self {
        let total_dur = 1200.0;
        let mut s = Self::empty(total_dur, source_duration);

        let num_points = 600;
        for _ in 0..num_points {
            let t = total_dur * rng.frand();
            let x = (75.0 * rng.frand()) as i32;
            let gf_val = (x * x) as f64; // Up to 5625 grains/sec
            s.gf.insert(t, gf_val);

            s.gf_rnd.insert(total_dur * rng.frand(), 500.0 * rng.frand());
            s.gl.insert(total_dur * rng.frand(), (4000.0 * rng.frand()) as f64);
            s.gl_rnd.insert(total_dur * rng.frand(), 500.0 * rng.frand());

            let pan = 3.0 * rng.frand() - 1.5;
            s.balance.insert(total_dur * rng.frand(), 1.0 + pan);
            s.balance_rnd.insert(total_dur * rng.frand(), 6.0 * rng.frand());
        }

        for _ in 0..num_points {
            let t = total_dur * rng.frand();
            let so_val = (source_duration - 0.05).max(0.0) * rng.frand();
            s.so.insert(t, so_val);

            let k = (20.0 * rng.frand() + 0.001) as i32;
            let so_rnd_val = ((k * k) as f64 * rng.frand()) / 1000.0;
            s.so_rnd.insert(total_dur * rng.frand(), so_rnd_val);

            let p1 = 100.0 * rng.frand();
            let p2 = 100.0 - p1;
            let pitch1 = 18.0 * rng.frand() - 36.0;
            let pitch2 = 12.0 * rng.frand() + 12.0;
            s.fm.insert(t, [pitch1, pitch2, 0.0, 0.0], [p1, p2, 0.0, 0.0]);
            s.fm_rnd.insert(total_dur * rng.frand(), 3.0 * rng.frand());
        }

        s.so.insert(0.0, 0.1);
        s.gl.insert(0.0, 200.0);
        s.attack.insert(0.0, 0.1);
        s.decay.insert(0.0, 0.1);
        s.gain.insert(15.0, 0.03);
        s.time_jitter.insert(0.0, 0.002);

        s
    }

    fn gen_flowing(source_duration: f64, rng: &mut FastRand) -> Self {
        let total_dur = 1200.0;
        let mut s = Self::empty(total_dur, source_duration);

        let num_points = 80; // 1 point per 15 seconds
        for _ in 0..num_points {
            let t = total_dur * rng.frand();
            let x = (75.0 * rng.frand()) as i32;
            let gf_val = (x * x) as f64;
            s.gf.insert(t, gf_val);

            s.gf_rnd.insert(total_dur * rng.frand(), 200.0 * rng.frand());
            s.gl.insert(total_dur * rng.frand(), 2500.0 * rng.frand() + 500.0);
            s.gl_rnd.insert(total_dur * rng.frand(), 300.0 * rng.frand());

            let pan = 2.0 * rng.frand() - 1.0;
            s.balance.insert(total_dur * rng.frand(), 1.0 + pan);
            s.balance_rnd.insert(total_dur * rng.frand(), 2.0 * rng.frand());
        }

        for _ in 0..num_points {
            let t = total_dur * rng.frand();
            let so_val = (source_duration - 0.05).max(0.0) * rng.frand();
            s.so.insert(t, so_val);
            s.so_rnd.insert(total_dur * rng.frand(), 0.05 * rng.frand());

            let p1 = 60.0;
            let p2 = 40.0;
            let pitch1 = (rng.frand() - 0.5) * 12.0;
            let pitch2 = (rng.frand() - 0.5) * 7.0;
            s.fm.insert(t, [pitch1, pitch2, 0.0, 0.0], [p1, p2, 0.0, 0.0]);
            s.fm_rnd.insert(total_dur * rng.frand(), 1.0 * rng.frand());
        }

        s.so.insert(0.0, 0.0);
        s.gl.insert(0.0, 1500.0);
        s.attack.insert(0.0, 0.25);
        s.decay.insert(0.0, 0.25);
        s.gain.insert(15.0, 0.025);
        s.time_jitter.insert(0.0, 0.005);

        s
    }

    fn gen_pentathon(source_duration: f64, rng: &mut FastRand) -> Self {
        let total_dur = 1200.0;
        let mut s = Self::empty(total_dur, source_duration);

        s.gf.insert(0.0, 2000.0);
        s.gf_rnd.insert(0.0, 100.0);
        s.gl.insert(0.0, 2000.0);
        s.attack.insert(0.0, 0.15);
        s.decay.insert(0.0, 0.15);
        s.gain.insert(0.0, 0.035);

        // Brownian walk on the 23 pentatonic scale notes!
        let num_steps = 150;
        let mut idx1 = rng.rand_int(PENTATONIC_SCALE.len() as i32) as usize;
        let mut idx2 = rng.rand_int(PENTATONIC_SCALE.len() as i32) as usize;

        for step in 0..num_steps {
            let t = (step as f64 / num_steps as f64) * total_dur;

            // Brownian step: -2, -1, 0, 1, 2
            let step1 = rng.rand_int(5) - 2;
            let step2 = rng.rand_int(5) - 2;
            idx1 = (idx1 as i32 + step1).clamp(0, PENTATONIC_SCALE.len() as i32 - 1) as usize;
            idx2 = (idx2 as i32 + step2).clamp(0, PENTATONIC_SCALE.len() as i32 - 1) as usize;

            let pitch1 = PENTATONIC_SCALE[idx1];
            let pitch2 = PENTATONIC_SCALE[idx2];
            s.fm.insert(t, [pitch1, pitch2, 0.0, 0.0], [50.0, 50.0, 0.0, 0.0]);

            s.balance.insert(t, if step % 2 == 0 { 0.4 } else { 1.6 });
            s.so.insert(t, (source_duration * (step as f64 / num_steps as f64)).min(source_duration));
        }

        s.time_jitter.insert(0.0, 0.001);
        s
    }

    fn gen_shepard(source_duration: f64, rng: &mut FastRand) -> Self {
        let total_dur = 1200.0;
        let mut s = Self::empty(total_dur, source_duration);

        s.gf.insert(0.0, 2000.0);
        s.gf_rnd.insert(0.0, 100.0);
        s.gl.insert(0.0, 2000.0);
        s.attack.insert(0.0, 0.1);
        s.decay.insert(0.0, 0.1);
        s.gain.insert(0.0, 0.03);

        // Circular pitch illusion across 4 octaves (-24, -12, 0, +12, +24)
        let cycles = 20;
        let points_per_cycle = 10;
        let total_points = cycles * points_per_cycle;

        for i in 0..total_points {
            let t = (i as f64 / total_points as f64) * total_dur;
            let phase = (i % points_per_cycle) as f64 / points_per_cycle as f64; // 0..1
            let semitones_glide = phase * 12.0;

            let v1 = -24.0 + semitones_glide;
            let v2 = -12.0 + semitones_glide;
            let v3 = 0.0 + semitones_glide;
            let v4 = 12.0 + semitones_glide;

            s.fm.insert(t, [v1, v2, v3, v4], [25.0, 25.0, 25.0, 25.0]);
            s.balance.insert(t, 1.0 + 0.5 * (phase * std::f64::consts::TAU).sin());
            s.so.insert(t, source_duration * rng.frand());
        }

        s.time_jitter.insert(0.0, 0.001);
        s
    }

    fn gen_dis(source_duration: f64, rng: &mut FastRand) -> Self {
        let total_dur = 1200.0;
        let mut s = Self::empty(total_dur, source_duration);

        let num_points = 133;
        for _ in 0..num_points {
            let t = total_dur * rng.frand();
            let x = (75.0 * rng.frand()) as i32;
            s.gf.insert(t, (x * x) as f64);
            s.gf_rnd.insert(total_dur * rng.frand(), 300.0 * rng.frand());
            s.gl.insert(total_dur * rng.frand(), 2000.0);

            let pan = 2.0 * rng.frand() - 1.0;
            s.balance.insert(total_dur * rng.frand(), 1.0 + pan);
        }

        for _ in 0..num_points {
            let t = total_dur * rng.frand();
            s.so.insert(t, (source_duration - 0.05).max(0.0) * rng.frand());

            // Pitch transpositions between -52 and -12 semitones (subterranean!)
            let pitch1 = -52.0 + 18.0 * rng.frand();
            let pitch2 = -12.0 + 10.0 * rng.frand();
            s.fm.insert(t, [pitch1, pitch2, 0.0, 0.0], [70.0, 30.0, 0.0, 0.0]);
            s.fm_rnd.insert(total_dur * rng.frand(), 2.0 * rng.frand());
        }

        s.gl.insert(0.0, 2000.0);
        s.attack.insert(0.0, 0.1);
        s.decay.insert(0.0, 0.1);
        s.gain.insert(0.0, 0.02);
        s.time_jitter.insert(0.0, 0.002);

        s
    }

    fn gen_stretch(
        source_duration: f64,
        target_dur: f64,
        with_octaves: bool,
        rng: &mut FastRand,
    ) -> Self {
        let mut s = Self::empty(target_dur, source_duration);

        s.gf.insert(0.0, 1500.0);
        s.gf_rnd.insert(0.0, 100.0);
        s.gl.insert(0.0, 3000.0);
        s.gl_rnd.insert(0.0, 100.0);
        s.attack.insert(0.0, 0.2);
        s.decay.insert(0.0, 0.2);
        s.gain.insert(0.0, 0.035);

        // Smooth continuous ramp of source offset from 0 to source_duration
        s.so.insert(0.0, 0.0);
        s.so.insert(target_dur, source_duration);

        if with_octaves {
            // Octaves stretch: 0, +12, +24 semitones
            let num_points = 50;
            for _ in 0..num_points {
                let t = target_dur * rng.frand();
                let p1 = 50.0 + 30.0 * rng.frand();
                let p2 = 30.0 * rng.frand();
                let p3 = 20.0 * rng.frand();
                s.fm.insert(t, [0.0, 12.0, 24.0, 0.0], [p1, p2, p3, 0.0]);
            }
        } else {
            s.fm.insert(0.0, [0.0, 0.0, 0.0, 0.0], [100.0, 0.0, 0.0, 0.0]);
        }

        s.balance.insert(0.0, 1.0);
        s.balance_rnd.insert(0.0, 0.5);
        s.time_jitter.insert(0.0, 0.001);

        s
    }

    fn gen_flash(source_duration: f64, rng: &mut FastRand) -> Self {
        let total_dur = 1200.0;
        let mut s = Self::empty(total_dur, source_duration);

        let num_points = 2400; // Hyper-hectic sprint!
        for _ in 0..num_points {
            let t = total_dur * rng.frand();
            let x = (75.0 * rng.frand()) as i32;
            s.gf.insert(t, (x * x) as f64);
            s.so.insert(t, source_duration * rng.frand());

            let p1 = 60.0;
            let p2 = 40.0;
            let pitch1 = 24.0 * rng.frand() - 12.0;
            let pitch2 = 12.0 * rng.frand() + 7.0;
            s.fm.insert(t, [pitch1, pitch2, 0.0, 0.0], [p1, p2, 0.0, 0.0]);
        }

        s.gl.insert(0.0, 800.0);
        s.attack.insert(0.0, 0.05);
        s.decay.insert(0.0, 0.05);
        s.gain.insert(0.0, 0.035);
        s.time_jitter.insert(0.0, 0.001);

        s
    }

    fn gen_slack(source_duration: f64, rng: &mut FastRand) -> Self {
        let total_dur = 1200.0;
        let mut s = Self::empty(total_dur, source_duration);

        let num_points = 40; // Extremely spacious and calm
        for _ in 0..num_points {
            let t = total_dur * rng.frand();
            let x = (30.0 * rng.frand()) as i32;
            s.gf.insert(t, (x * x).max(25) as f64);
            s.gl.insert(t, 4000.0 * rng.frand() + 1000.0);
            s.so.insert(t, source_duration * rng.frand());

            let pitch1 = (rng.frand() - 0.5) * 7.0;
            s.fm.insert(t, [pitch1, 0.0, 0.0, 0.0], [100.0, 0.0, 0.0, 0.0]);
            s.balance.insert(t, 1.0 + (rng.frand() - 0.5) * 1.5);
        }

        s.gl.insert(0.0, 3000.0);
        s.attack.insert(0.0, 0.35);
        s.decay.insert(0.0, 0.35);
        s.gain.insert(0.0, 0.04);
        s.time_jitter.insert(0.0, 0.01);

        s
    }

    fn empty(total_duration: f64, source_duration: f64) -> Self {
        Self {
            total_duration,
            source_duration,
            gf: BreakPointList::new(),
            gf_rnd: BreakPointList::new(),
            gl: BreakPointList::new(),
            gl_rnd: BreakPointList::new(),
            attack: BreakPointList::new(),
            attack_rnd: BreakPointList::new(),
            decay: BreakPointList::new(),
            decay_rnd: BreakPointList::new(),
            gain: BreakPointList::new(),
            gain_rnd: BreakPointList::new(),
            so: BreakPointList::new(),
            so_rnd: BreakPointList::new(),
            balance: BreakPointList::new(),
            balance_rnd: BreakPointList::new(),
            time_jitter: BreakPointList::new(),
            fm: FMList::new(),
            fm_rnd: BreakPointList::new(),
        }
    }
}
