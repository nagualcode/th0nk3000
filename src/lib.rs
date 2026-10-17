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
    pub last_seed_param: i64,
    pub left: Vec<f32>,
    pub right: Vec<f32>,
}

impl Default for ThonkDspState {
    fn default() -> Self {
        Self {
            engine: GranularEngine::new(44100.0),
            last_trigger_count: 0,
            last_score: ScoreType::ClassicFlowing,
            last_seed_param: 0,
            left: vec![0.0; 4096],
            right: vec![0.0; 4096],
        }
    }
}

fn seed_from_param(v: i64) -> Option<u32> {
    (v > 0).then_some(v as u32)
}

/// The main Th0nk plugin logic descriptor.
pub struct ThonkPlugin;

impl PluginLogic for ThonkPlugin {
    type Params = ThonkParams;
    type DspState = ThonkDspState;

    fn reset(state: &mut Self::DspState, params: &Self::Params, config: &AudioConfig) {
        let n = config.max_block_size.max(64);
        if state.left.len() < n {
            state.left.resize(n, 0.0);
            state.right.resize(n, 0.0);
        }
        state.engine.set_host_sample_rate(config.sample_rate);
        let score = params.score.value();
        state.last_score = score;
        state.last_seed_param = params.seed.value();
        state
            .engine
            .trigger(score, seed_from_param(state.last_seed_param));
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

        // New audio file from the UI (non-blocking). The old buffer is handed back
        // so it is freed on the UI thread, not here.
        if let Ok(mut pending_guard) = params.pending_file.try_write() {
            if let Some(new_buf) = pending_guard.take() {
                let old = state.engine.set_source(new_buf);
                if let Ok(mut retired) = params.retired_file.try_write() {
                    *retired = Some(old);
                }
            }
        }

        state.engine.set_live(params.live_input.value());

        // THONK button, score change or seed change: new composition.
        let trig = params.trigger_counter.load(Ordering::Relaxed);
        let cur_score = params.score.value();
        let seed_param = params.seed.value();
        if trig != state.last_trigger_count
            || cur_score != state.last_score
            || seed_param != state.last_seed_param
        {
            state.last_trigger_count = trig;
            state.last_score = cur_score;
            state.last_seed_param = seed_param;
            state.engine.trigger(cur_score, seed_from_param(seed_param));
        }

        if params.live_input.value() && buffer.num_input_channels() >= 1 {
            let inp_l = buffer.input(0);
            let inp_r = if buffer.num_input_channels() >= 2 {
                buffer.input(1)
            } else {
                inp_l
            };
            state.engine.feed_live_input(inp_l, inp_r);
        }

        let wrapping = params.wrapping_distortion.value();
        let density = params.density.value() / 100.0;
        let gain = params.gain.read();

        let mut done = 0;
        while done < num_samples {
            let n = (num_samples - done).min(state.left.len());
            let (l, r) = (&mut state.left[..n], &mut state.right[..n]);
            state.engine.process_block(l, r, wrapping, density, gain);
            if buffer.num_output_channels() >= 1 {
                buffer.output(0)[done..done + n].copy_from_slice(l);
            }
            if buffer.num_output_channels() >= 2 {
                buffer.output(1)[done..done + n].copy_from_slice(r);
            }
            done += n;
        }

        params
            .playing_seed
            .store(state.engine.last_seed, Ordering::Relaxed);
        params
            .playing_score
            .store(state.engine.resolved_score().to_index() as u32, Ordering::Relaxed);

        if buffer.num_output_channels() >= 1 {
            context.set_meter(P::MeterLeft, buffer.output_peak(0));
        }
        if buffer.num_output_channels() >= 2 {
            context.set_meter(P::MeterRight, buffer.output_peak(1));
        }
        context.set_meter(
            P::MeterDensity,
            (state.engine.active_grain_count as f32 / engine::DENSITY_METER_MAX as f32)
                .clamp(0.0, 1.0),
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
}
