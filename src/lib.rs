//! thOnk_0+2 for Modern macOS
//! Reverse-engineered and recreated using the Truce Audio framework.
//!
//! Original application: "Th0nk" by Audio Ease (Arjen van der Schoot, UI by antiorp).
//! Synthesis technique: Asynchronous Granular Synthesis with 16-bit integer wrapping overflow.

pub mod aiff;
pub mod engine;
pub mod params;
pub mod score;
pub mod source_slot;
pub mod ui;

use aiff::AudioBufferData;
use engine::GranularEngine;
use params::{ThonkParams, ThonkParamsParamId as P, GRAIN_TRAIL};
use score::{Length, ScoreType};
use std::sync::atomic::Ordering;
use std::sync::Arc;
use truce::prelude::*;
use truce_egui::EguiEditor;

/// Samples a changed Length setting must stay put before the score is rebuilt
/// (so dragging the knob does not regenerate on every block).
const LENGTH_DEBOUNCE_SAMPLES: usize = 4800;

/// Hot-reloadable DSP State holding the granular engine.
pub struct ThonkDspState {
    pub engine: GranularEngine,
    pub left: Vec<f32>,
    pub right: Vec<f32>,
    empty_source: Arc<AudioBufferData>,
    sample_rate: f64,
    audio_generation: u64,
    last_trigger: u32,
    last_play: u32,
    last_stop: u32,
    last_seek: u32,
    last_score: ScoreType,
    last_seed_param: i64,
    pending_length: Option<(Length, usize)>,
    host_was_playing: bool,
    host_expected_pos: f64,
}

impl Default for ThonkDspState {
    fn default() -> Self {
        Self {
            engine: GranularEngine::new(44100.0),
            left: vec![0.0; 4096],
            right: vec![0.0; 4096],
            empty_source: Arc::new(AudioBufferData::default()),
            sample_rate: 44100.0,
            audio_generation: u64::MAX,
            last_trigger: 0,
            last_play: 0,
            last_stop: 0,
            last_seek: 0,
            last_score: ScoreType::ClassicFlowing,
            last_seed_param: 0,
            pending_length: None,
            host_was_playing: false,
            host_expected_pos: 0.0,
        }
    }
}

fn seed_from_param(v: i64) -> Option<u32> {
    (v > 0).then_some(v as u32)
}

fn length_from(params: &ThonkParams) -> Length {
    Length {
        mode: params.length_mode.value(),
        secs: params.length.value() as f64,
    }
}

/// The main Th0nk plugin logic descriptor.
pub struct ThonkPlugin;

impl ThonkPlugin {
    /// Applies settings and editor requests; everything here is allocation-free.
    fn apply_controls(state: &mut ThonkDspState, params: &ThonkParams, n: usize) {
        let engine = &mut state.engine;

        // New source from the editor or from a restored project.
        let generation = params.audio.generation();
        if generation != state.audio_generation {
            if let Some(src) = params.audio.try_get() {
                state.audio_generation = generation;
                let new = src.unwrap_or_else(|| state.empty_source.clone());
                let old = engine.set_source(new);
                if let Ok(mut retired) = params.retired.try_write() {
                    *retired = Some(old);
                }
            }
        }

        engine.set_live(params.live_input.value());
        engine.set_density(params.density.value() as f64);
        engine.set_skip_silence(params.skip_silence.value());

        // Length: rebuild once the setting has been stable for a moment.
        let wanted = length_from(params);
        match state.pending_length {
            Some((len, ref mut waited)) if len == wanted => {
                *waited += n;
                if *waited >= LENGTH_DEBOUNCE_SAMPLES {
                    engine.set_length(len);
                    state.pending_length = None;
                }
            }
            _ => state.pending_length = Some((wanted, 0)),
        }

        // Score or seed changed: a new composition (keeps the transport state).
        let score = params.score.value();
        let seed_param = params.seed.value();
        let seed_changed = seed_param != state.last_seed_param
            && seed_param != 0
            && seed_param as u32 != engine.seed();
        if score != state.last_score || seed_changed {
            engine.trigger(score, seed_from_param(seed_param));
        }
        state.last_score = score;
        state.last_seed_param = seed_param;

        // Editor buttons.
        let trig = params.trigger_counter.load(Ordering::Relaxed);
        if trig != state.last_trigger {
            state.last_trigger = trig;
            engine.trigger(score, seed_from_param(seed_param));
            engine.play();
        }
        let play = params.play_counter.load(Ordering::Relaxed);
        if play != state.last_play {
            state.last_play = play;
            engine.play();
        }
        let stop = params.stop_counter.load(Ordering::Relaxed);
        if stop != state.last_stop {
            state.last_stop = stop;
            engine.stop();
        }
        let seek = params.seek_counter.load(Ordering::Relaxed);
        if seek != state.last_seek {
            state.last_seek = seek;
            engine.seek(params.seek_ms.load(Ordering::Relaxed) as f64 / 1000.0);
        }
    }

    /// Host Sync: start/stop with the host and follow its position.
    fn follow_host(state: &mut ThonkDspState, params: &ThonkParams, transport: &TransportInfo, n: usize) -> bool {
        let engine = &mut state.engine;
        let host_playing = params.host_sync.value() && transport.playing;
        if host_playing {
            let pos = transport.position_seconds.max(0.0);
            if !state.host_was_playing || (pos - state.host_expected_pos).abs() > 0.1 {
                engine.seek(pos);
            }
            engine.auto_restart = false;
            engine.play();
            state.host_expected_pos = pos + n as f64 / state.sample_rate;
        } else if state.host_was_playing {
            engine.stop();
            engine.auto_restart = true;
        }
        state.host_was_playing = host_playing;
        host_playing
    }

    fn publish(state: &ThonkDspState, params: &ThonkParams, synced: bool) {
        let e = &state.engine;
        let st = &params.status;
        st.seed.store(e.seed(), Ordering::Relaxed);
        st.score.store(e.resolved_score().to_index() as u32, Ordering::Relaxed);
        st.playing.store(e.is_playing(), Ordering::Relaxed);
        st.synced.store(synced, Ordering::Relaxed);
        st.position_ms.store((e.position_secs() * 1000.0) as u32, Ordering::Relaxed);
        st.total_ms.store((e.total_duration() * 1000.0) as u32, Ordering::Relaxed);
        st.source_ms.store((e.source_duration() * 1000.0) as u32, Ordering::Relaxed);
        st.layers.store(e.active_grain_count as u32, Ordering::Relaxed);
        for (i, g) in e.grain_trail().enumerate().take(GRAIN_TRAIL) {
            let packed = ((g.source.clamp(0.0, 1.0) * 65535.0) as u32) << 16
                | ((g.balance.clamp(0.0, 1.0) * 255.0) as u32) << 8;
            st.grains[i].store(packed, Ordering::Relaxed);
        }
    }
}

impl PluginLogic for ThonkPlugin {
    type Params = ThonkParams;
    type DspState = ThonkDspState;

    fn reset(state: &mut Self::DspState, params: &Self::Params, config: &AudioConfig) {
        let n = config.max_block_size.max(64);
        if state.left.len() < n {
            state.left.resize(n, 0.0);
            state.right.resize(n, 0.0);
        }
        state.sample_rate = config.sample_rate;
        state.engine.set_host_sample_rate(config.sample_rate);
        state.last_score = params.score.value();
        state.last_seed_param = params.seed.value();
        state.engine.set_length(length_from(params));
        state
            .engine
            .trigger(state.last_score, seed_from_param(state.last_seed_param));
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

        Self::apply_controls(state, params, num_samples);
        let synced = Self::follow_host(state, params, context.transport, num_samples);

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
        let gain = params.gain.read();

        let mut done = 0;
        while done < num_samples {
            let n = (num_samples - done).min(state.left.len());
            let (l, r) = (&mut state.left[..n], &mut state.right[..n]);
            state.engine.process_block(l, r, wrapping, gain);
            if buffer.num_output_channels() >= 1 {
                buffer.output(0)[done..done + n].copy_from_slice(l);
            }
            if buffer.num_output_channels() >= 2 {
                buffer.output(1)[done..done + n].copy_from_slice(r);
            }
            done += n;
        }

        Self::publish(state, params, synced);

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
    fn opens_stopped_and_silent() {
        let result = driver!(Plugin)
            .sample_rate(44_100.0)
            .channels(2)
            .block_size(256)
            .duration(Duration::from_millis(500))
            .input(InputSource::Silence)
            .run();
        assertions::assert_no_nans(&result);
        assertions::assert_silence(&result);
    }

    #[test]
    fn follows_host_transport() {
        let result = driver!(Plugin)
            .sample_rate(48_000.0)
            .channels(2)
            .block_size(256)
            .duration(Duration::from_millis(1500))
            .input(InputSource::Silence)
            .playing(true)
            .setup(|plugin, _| {
                plugin
                    .params()
                    .audio
                    .set(Some(Arc::new(AudioBufferData::demo())));
            })
            .run();
        assertions::assert_no_nans(&result);
        assertions::assert_nonzero(&result);
    }

    /// Renders the editor with a loaded sound and a playing state, for the README.
    /// `cargo test --release --lib readme_screenshots -- --ignored`
    #[test]
    #[ignore]
    fn readme_screenshots() {
        use truce_core::editor::Editor;
        let params = Arc::new(ThonkParams::new());
        params.audio.set(Some(Arc::new(AudioBufferData::demo())));
        params.score.set_value(ScoreType::ClassicHectic);
        let st = &params.status;
        st.seed.store(418_203, Ordering::Relaxed);
        st.score.store(ScoreType::ClassicHectic.to_index() as u32, Ordering::Relaxed);
        st.playing.store(true, Ordering::Relaxed);
        st.position_ms.store(252_000, Ordering::Relaxed);
        st.total_ms.store(1_200_000, Ordering::Relaxed);
        st.source_ms.store(2_500, Ordering::Relaxed);
        st.layers.store(312, Ordering::Relaxed);
        let mut rng = score::MacRandom::new(9);
        for g in &st.grains {
            let pos = (rng.frand() * 0.55 + 0.05) as f32;
            let bal = rng.frand() as f32;
            g.store(((pos * 65535.0) as u32) << 16 | ((bal * 255.0) as u32) << 8, Ordering::Relaxed);
        }
        std::fs::create_dir_all("docs").unwrap();
        for (theme, file) in [(0u8, "docs/screenshot.png"), (1, "docs/screenshot-classic.png")] {
            *params.theme.write().unwrap() = theme;
            let mut editor = ThonkPlugin::editor(params.clone());
            if let Some((px, w, h)) = editor.screenshot(params.clone()) {
                truce_test::save_png(std::path::Path::new(file), &px, w, h);
            }
        }
    }
}
