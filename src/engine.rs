//! High-density granular synthesis engine and accumulator.
//! Recreates Audio Ease's Th0nk synthesis architecture with 16-bit wrapping distortion.

use crate::aiff::AudioBufferData;
use crate::score::{FastRand, ScoreState, ScoreType};
use std::f64::consts::PI;

/// A single granular synthesis grain.
#[derive(Clone)]
pub struct Grain {
    pub duration_samples: usize,
    pub attack_samples: usize,
    pub decay_samples: usize,
    pub gain: f32,
    pub freq_multiplier: f64,
    pub pan_l: f32,
    pub pan_r: f32,
    pub source_pos_start: f64,
    pub progress: usize,
}

impl Grain {
    #[inline(always)]
    pub fn is_finished(&self) -> bool {
        self.progress >= self.duration_samples
    }

    /// Renders next stereo sample (left, right) for this grain.
    #[inline(always)]
    pub fn process_sample(&mut self, source: &[f32]) -> (f32, f32) {
        if self.progress >= self.duration_samples || source.is_empty() {
            return (0.0, 0.0);
        }

        // Raised-cosine (Hann) envelope calculation
        let env = if self.progress < self.attack_samples && self.attack_samples > 0 {
            let phase = (self.progress as f64 / self.attack_samples as f64) * PI;
            0.5 * (1.0 - phase.cos()) as f32
        } else if self.progress >= self.duration_samples.saturating_sub(self.decay_samples)
            && self.decay_samples > 0
        {
            let remaining = self.duration_samples - self.progress;
            let phase = (remaining as f64 / self.decay_samples as f64) * PI;
            0.5 * (1.0 - phase.cos()) as f32
        } else {
            1.0
        };

        // Linear interpolation resampling from source audio buffer
        let src_pos = self.source_pos_start + (self.progress as f64 * self.freq_multiplier);
        let src_len = source.len();

        let sample = if src_len > 1 {
            let idx0 = (src_pos as usize) % src_len;
            let idx1 = (idx0 + 1) % src_len;
            let frac = (src_pos - src_pos.floor()) as f32;
            source[idx0] + frac * (source[idx1] - source[idx0])
        } else if src_len == 1 {
            source[0]
        } else {
            0.0
        };

        self.progress += 1;

        let amp = sample * env * self.gain;
        (amp * self.pan_l, amp * self.pan_r)
    }
}

/// The core Granular Engine managing grain scheduling, parameter modulation, and accumulation.
pub struct GranularEngine {
    pub source: AudioBufferData,
    pub live_buffer: Vec<f32>,
    pub live_write_pos: usize,
    pub score_state: ScoreState,
    pub current_time_secs: f64,
    pub sample_rate: f64,
    pub grain_accumulator: f64,
    pub active_grains: Vec<Grain>,
    pub rng: FastRand,
    pub is_active: bool,
    pub active_grain_count: usize,
}

impl GranularEngine {
    pub fn new(sample_rate: f64) -> Self {
        let default_source = AudioBufferData::default_chime(sample_rate);
        let source_dur = default_source.samples.len() as f64 / sample_rate;
        let score_state = ScoreState::generate(ScoreType::ClassicFlowing, source_dur, 0x4170);

        Self {
            source: default_source,
            live_buffer: vec![0.0; (sample_rate * 5.0) as usize], // 5 second rolling ring buffer
            live_write_pos: 0,
            score_state,
            current_time_secs: 0.0,
            sample_rate,
            grain_accumulator: 0.0,
            active_grains: Vec::with_capacity(1024),
            rng: FastRand::new(0x4170),
            is_active: true,
            active_grain_count: 0,
        }
    }

    /// Loads a new audio file into the engine and resets the composition score.
    pub fn set_source(&mut self, data: AudioBufferData, score_type: ScoreType) {
        let dur = data.samples.len() as f64 / self.sample_rate;
        self.source = data;
        self.trigger_score(score_type, dur);
    }

    /// Triggers/regenerates a new score composition.
    pub fn trigger_score(&mut self, score_type: ScoreType, source_dur: f64) {
        let seed = self.rng.rand_int(0x7fffffff) as u32;
        self.score_state = ScoreState::generate(score_type, source_dur, seed);
        self.current_time_secs = 0.0;
        self.grain_accumulator = 0.0;
        self.active_grains.clear();
        self.is_active = true;
    }

    /// Pushes incoming live audio from DAW or microphone into ring buffer.
    pub fn feed_live_input(&mut self, left: &[f32], right: &[f32]) {
        let len = self.live_buffer.len();
        for i in 0..left.len() {
            let s = 0.5 * (left[i] + right.get(i).copied().unwrap_or(left[i]));
            self.live_buffer[self.live_write_pos] = s;
            self.live_write_pos = (self.live_write_pos + 1) % len;
        }
    }

    /// Renders a block of stereo audio frames into `out_left` and `out_right`.
    pub fn process_block(
        &mut self,
        out_left: &mut [f32],
        out_right: &mut [f32],
        wrapping_distortion: bool,
        density_multiplier: f32,
        gain_db: f32,
        use_live_input: bool,
    ) {
        if !self.is_active {
            out_left.fill(0.0);
            out_right.fill(0.0);
            return;
        }

        let num_samples = out_left.len();
        let dt = 1.0 / self.sample_rate;
        let master_gain = 10.0f32.powf(gain_db / 20.0);

        let source_slice: &[f32] = if use_live_input {
            &self.live_buffer
        } else {
            &self.source.samples
        };

        for i in 0..num_samples {
            let t = self.current_time_secs;

            // Stop if reached 20 minutes (1200 seconds) like original Th0nk
            if t >= self.score_state.total_duration {
                // Loop or restart composition
                self.current_time_secs = 0.0;
            }

            // Granular Frequency: grains per second
            let gf = self.score_state.get_granular_freq(t, &mut self.rng) * (density_multiplier as f64);
            self.grain_accumulator += gf * dt;

            // Spawn new grains when accumulator fills
            while self.grain_accumulator >= 1.0 {
                self.grain_accumulator -= 1.0;

                // Max vertical density: up to 600 concurrent grains
                if self.active_grains.len() < 600 {
                    let grain_len = self.score_state.get_grain_length(t, &mut self.rng);
                    let attack_pct = self.score_state.get_attack(t, &mut self.rng);
                    let decay_pct = self.score_state.get_decay(t, &mut self.rng);
                    let gain = self.score_state.get_gain(t, &mut self.rng) as f32;
                    let freq_mult = self.score_state.get_freq_multiplier(t, &mut self.rng);
                    let balance = self.score_state.get_balance(t, &mut self.rng);

                    // Constant-power stereo panning from reverse-engineered formula:
                    // pan_l = sqrt(pan * 0.5), pan_r = sqrt(1 - pan * 0.5)
                    let pan_norm = (balance * 0.5).clamp(0.0, 1.0);
                    let pan_l = (pan_norm).sqrt() as f32;
                    let pan_r = (1.0 - pan_norm).sqrt() as f32;

                    let so_secs = self.score_state.get_source_offset(t, &mut self.rng);
                    let jitter = self.score_state.get_time_jitter(t, &mut self.rng);
                    let start_sample_f = ((so_secs + jitter) * self.sample_rate).max(0.0);

                    let attack_samples = ((grain_len as f64) * attack_pct) as usize;
                    let decay_samples = ((grain_len as f64) * decay_pct) as usize;

                    self.active_grains.push(Grain {
                        duration_samples: grain_len,
                        attack_samples,
                        decay_samples,
                        gain,
                        freq_multiplier: freq_mult,
                        pan_l,
                        pan_r,
                        source_pos_start: start_sample_f,
                        progress: 0,
                    });
                }
            }

            // Accumulate active grains
            if wrapping_distortion {
                // AUTHENTIC 16-BIT INTEGER WRAPPING DISTORTION (Audio Ease Th0nk 1995 behavior)
                // "if thOnk_0+2 goes over 0dB Thonk wraps. Compared to which clipping is a picnic."
                let mut acc_l_i16: i16 = 0;
                let mut acc_r_i16: i16 = 0;

                for grain in &mut self.active_grains {
                    let (gl, gr) = grain.process_sample(source_slice);
                    let gl_i16 = (gl * 32767.0).clamp(-32768.0, 32767.0) as i16;
                    let gr_i16 = (gr * 32767.0).clamp(-32768.0, 32767.0) as i16;

                    // Wrapping addition causes digital wrap distortion past 0 dBFS
                    acc_l_i16 = acc_l_i16.wrapping_add(gl_i16);
                    acc_r_i16 = acc_r_i16.wrapping_add(gr_i16);
                }

                out_left[i] = (acc_l_i16 as f32 / 32768.0) * master_gain;
                out_right[i] = (acc_r_i16 as f32 / 32768.0) * master_gain;
            } else {
                // Clean mode: float accumulation with smooth soft-clipping
                let mut sum_l = 0.0f32;
                let mut sum_r = 0.0f32;

                for grain in &mut self.active_grains {
                    let (gl, gr) = grain.process_sample(source_slice);
                    sum_l += gl;
                    sum_r += gr;
                }

                out_left[i] = (sum_l * master_gain).tanh();
                out_right[i] = (sum_r * master_gain).tanh();
            }

            // Remove finished grains
            self.active_grains.retain(|g| !g.is_finished());

            self.current_time_secs += dt;
        }

        self.active_grain_count = self.active_grains.len();
    }
}

/// Renders an entire score offline to a 16-bit stereo WAV file at maximum CPU speed.
/// Emulates the original Mac OS 9 Th0nk workflow of calculating the file directly to disk.
pub fn offline_render_to_file<F: Fn(f32, &str)>(
    source: AudioBufferData,
    score_type: ScoreType,
    wrapping: bool,
    density_mult: f32,
    gain_db: f32,
    sample_rate: f64,
    output_path: &std::path::Path,
    progress_callback: F,
) -> Result<(), String> {
    let mut engine = GranularEngine::new(sample_rate);
    let dur = source.samples.len() as f64 / source.sample_rate;
    engine.set_source(source, score_type);
    engine.trigger_score(score_type, dur);

    let total_duration_secs = engine.score_state.total_duration;
    let total_frames = (total_duration_secs * sample_rate) as usize;

    let spec = hound::WavSpec {
        channels: 2,
        sample_rate: sample_rate as u32,
        bits_per_sample: 16,
        sample_format: hound::SampleFormat::Int,
    };
    let mut writer = hound::WavWriter::create(output_path, spec).map_err(|e| e.to_string())?;

    let block_size = 2048;
    let mut left_buf = vec![0.0f32; block_size];
    let mut right_buf = vec![0.0f32; block_size];

    let mut frames_rendered = 0;
    while frames_rendered < total_frames {
        let chunk = block_size.min(total_frames - frames_rendered);
        engine.process_block(
            &mut left_buf[..chunk],
            &mut right_buf[..chunk],
            wrapping,
            density_mult,
            gain_db,
            false,
        );

        for i in 0..chunk {
            let sl = (left_buf[i] * 32767.0).clamp(-32768.0, 32767.0) as i16;
            let sr = (right_buf[i] * 32767.0).clamp(-32768.0, 32767.0) as i16;
            writer.write_sample(sl).map_err(|e| e.to_string())?;
            writer.write_sample(sr).map_err(|e| e.to_string())?;
        }

        frames_rendered += chunk;
        if frames_rendered % (block_size * 50) == 0 || frames_rendered >= total_frames {
            let prog = frames_rendered as f32 / total_frames as f32;
            let status = format!(
                "Rendering: {:.0}% ({:.1}s / {:.1}s) [{} layers]",
                prog * 100.0,
                frames_rendered as f64 / sample_rate,
                total_duration_secs,
                engine.active_grain_count
            );
            progress_callback(prog, &status);
        }
    }
    writer.finalize().map_err(|e| e.to_string())?;
    Ok(())
}

