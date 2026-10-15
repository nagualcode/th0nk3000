//! thOnk_0+2 for Modern macOS
//! Reverse-engineered and recreated using the Truce Audio framework.
//!
//! Original application: "Th0nk" by Audio Ease (Arjen van der Schoot, UI by antiorp).
//! Synthesis technique: Asynchronous Granular Synthesis with 16-bit integer wrapping overflow.

pub mod aiff;
pub mod engine;
pub mod params;
pub mod score;
pub mod ui;

use engine::GranularEngine;
use params::{ThonkParams, ThonkParamsParamId as P};
use score::ScoreType;
use std::sync::atomic::Ordering;
use std::sync::Arc;
use truce::prelude::*;
use truce_egui::EguiEditor;
use truce_font::JETBRAINS_MONO;

/// Hot-reloadable DSP State holding the granular engine.
pub struct ThonkDspState {
    pub engine: GranularEngine,
    pub last_trigger_count: u32,
    pub last_score: ScoreType,
}

impl Default for ThonkDspState {
    fn default() -> Self {
        Self {
            engine: GranularEngine::new(44100.0),
            last_trigger_count: 0,
            last_score: ScoreType::ClassicFlowing,
        }
    }
}

/// The main Th0nk plugin logic descriptor.
pub struct ThonkPlugin;

impl PluginLogic for ThonkPlugin {
    type Params = ThonkParams;
    type DspState = ThonkDspState;

    fn reset(state: &mut Self::DspState, _params: &Self::Params, config: &AudioConfig) {
        state.engine.sample_rate = config.sample_rate;
    }

    fn process(
        state: &mut Self::DspState,
        params: &Self::Params,
        buffer: &mut AudioBuffer,
        _events: &EventList,
        context: &mut ProcessContext,
    ) -> ProcessStatus {
        let num_samples = buffer.num_samples();
        if num_samples == 0 {
            return ProcessStatus::Normal;
        }

        // Check for new loaded audio file (non-blocking)
        if let Ok(mut pending_guard) = params.pending_file.try_write() {
            if let Some(new_buf) = pending_guard.take() {
                state.engine.set_source(new_buf, params.score.value());
            }
        }

        // Check for THONK trigger click
        let trig = params.trigger_counter.load(Ordering::Relaxed);
        if trig != state.last_trigger_count {
            state.last_trigger_count = trig;
            let dur = state.engine.source.samples.len() as f64 / state.engine.sample_rate;
            state.engine.trigger_score(params.score.value(), dur);
        }

        // Check if score changed
        let cur_score = params.score.value();
        if cur_score != state.last_score {
            state.last_score = cur_score;
            let dur = state.engine.source.samples.len() as f64 / state.engine.sample_rate;
            state.engine.trigger_score(cur_score, dur);
        }

        let use_live = params.live_input.value();
        let wrapping = params.wrapping_distortion.value();
        let density = params.density.value();
        let gain = params.gain.read();

        // Feed live input if enabled
        if use_live && buffer.num_input_channels() >= 1 {
            let inp_l = buffer.input(0);
            let inp_r = if buffer.num_input_channels() >= 2 {
                buffer.input(1)
            } else {
                inp_l
            };
            state.engine.feed_live_input(inp_l, inp_r);
        }

        // Allocate temporary stereo buffers for granular rendering
        let mut left_buf = vec![0.0f32; num_samples];
        let mut right_buf = vec![0.0f32; num_samples];

        state.engine.process_block(
            &mut left_buf,
            &mut right_buf,
            wrapping,
            density / 100.0, // Convert percentage to multiplier
            gain,
            use_live,
        );

        // Copy rendered audio to output channels
        if buffer.num_output_channels() >= 1 {
            buffer.output(0).copy_from_slice(&left_buf);
        }
        if buffer.num_output_channels() >= 2 {
            buffer.output(1).copy_from_slice(&right_buf);
        }

        // Update real-time meters
        if buffer.num_output_channels() >= 1 {
            context.set_meter(P::MeterLeft, buffer.output_peak(0));
        }
        if buffer.num_output_channels() >= 2 {
            context.set_meter(P::MeterRight, buffer.output_peak(1));
        }
        context.set_meter(
            P::MeterDensity,
            (state.engine.active_grain_count as f32 / 600.0).clamp(0.0, 1.0),
        );

        ProcessStatus::Normal
    }

    fn editor(params: Arc<Self::Params>) -> Box<dyn Editor> {
        EguiEditor::new(params.clone(), (ui::WINDOW_W, ui::WINDOW_H), ui::thonk_ui)
            .with_visuals(truce_egui::theme::dark())
            .with_font(JETBRAINS_MONO)
            .resizable(true)
            .min_size((ui::MIN_W, ui::MIN_H))
            .max_size((ui::MAX_W, ui::MAX_H))
            .into_editor()
    }
}

truce::plugin! {
    logic: ThonkPlugin,
    params: ThonkParams,
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::time::Duration;
    use truce_test::{assertions, driver, InputSource};

    #[test]
    fn test_driver_renders_audio() {
        let result = driver!(Plugin)
            .sample_rate(44_100.0)
            .channels(2)
            .block_size(256)
            .duration(Duration::from_millis(500))
            .input(InputSource::Silence)
            .run();

        assertions::assert_no_nans(&result);
        assertions::assert_nonzero(&result);
    }

    #[test]
    fn test_all_twelve_scores_generate() {
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
            ScoreType::Stochastic,
        ];

        for score in scores {
            let state = score::ScoreState::generate(score, 5.0, 12345);
            let mut rng = score::FastRand::new(54321);
            let gf = state.get_granular_freq(1.0, &mut rng);
            let gl = state.get_grain_length(1.0, &mut rng);
            let gain = state.get_gain(1.0, &mut rng);
            let mult = state.get_freq_multiplier(1.0, &mut rng);

            assert!(gf >= 1.0, "Granular freq must be >= 1 for {:?}", score);
            assert!(gl >= 16, "Grain len must be valid for {:?}", score);
            assert!(gain >= 0.0, "Gain must be non-negative for {:?}", score);
            assert!(mult > 0.0, "Frequency multiplier must be positive for {:?}", score);
        }
    }

    #[test]
    fn test_wrapping_distortion_arithmetic() {
        // 16-bit integer wrap-around verification
        let val1: i16 = 25000;
        let val2: i16 = 20000;
        let wrapped = val1.wrapping_add(val2);
        // 45000 overflows signed 16-bit, wraps to negative:
        assert_eq!(wrapped, -20536);
        let normalized = wrapped as f32 / 32768.0;
        assert!(normalized < 0.0, "Wrapped sample must be in negative half-cycle");
    }

    #[test]
    fn test_raised_cosine_envelope() {
        let mut grain = engine::Grain {
            duration_samples: 100,
            attack_samples: 20,
            decay_samples: 20,
            gain: 1.0,
            freq_multiplier: 1.0,
            pan_l: 0.707,
            pan_r: 0.707,
            source_pos_start: 0.0,
            progress: 0,
        };

        let dummy_source = vec![1.0f32; 1000];

        // Sample 0: at start of attack, envelope ~ 0.0
        let (l0, _) = grain.process_sample(&dummy_source);
        assert!(l0.abs() < 0.05, "Attack starts near zero");

        // Fast forward to sustain (sample 50)
        grain.progress = 50;
        let (l50, _) = grain.process_sample(&dummy_source);
        assert!((l50 - 0.707).abs() < 0.05, "Sustain is at full amplitude");

        // Fast forward to end of decay (sample 99)
        grain.progress = 99;
        let (l99, _) = grain.process_sample(&dummy_source);
        assert!(l99 < 0.15, "Decay finishes near zero");
    }

    #[test]
    fn test_constant_power_panning() {
        for pan_int in 0..=100 {
            let balance = pan_int as f64 / 50.0; // 0.0 to 2.0
            let pan_norm = (balance * 0.5).clamp(0.0, 1.0);
            let pan_l = pan_norm.sqrt() as f32;
            let pan_r = (1.0 - pan_norm).sqrt() as f32;
            let power = pan_l * pan_l + pan_r * pan_r;
            assert!((power - 1.0).abs() < 1e-4, "Pan must be constant power: {}", power);
        }
    }
}

