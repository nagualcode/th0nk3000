//! Plugin parameters, persisted settings and the lock-free status the audio
//! thread publishes for the editor.

use crate::aiff::AudioBufferData;
use crate::score::{LengthMode, ScoreType};
use crate::source_slot::SourceSlot;
use std::sync::atomic::{AtomicBool, AtomicU32, Ordering};
use std::sync::{Arc, RwLock};
use truce::prelude::*;

/// Number of recent grains the engine reports for the waveform display.
pub const GRAIN_TRAIL: usize = 96;

#[derive(Params)]
pub struct ThonkParams {
    #[param(name = "Method")]
    pub score: EnumParam<ScoreType>,

    #[param(name = "16-Bit Wrap Clip", default = 1)]
    pub wrapping_distortion: BoolParam,

    #[param(name = "Live Audio Input", default = 0)]
    pub live_input: BoolParam,

    /// Grain-rate multiplier (1.0 = the score's own density). Same normalised
    /// mapping as the old 20..300 range, so older projects keep their value.
    #[param(
        name = "Density",
        range = "linear(0.2, 3.0)",
        default = 1.0,
        unit = "%"
    )]
    pub density: FloatParam,

    /// 0 = a new random composition on every Thonk (the original seeded from the
    /// clock); any other value recalls the same composition.
    #[param(name = "Seed", range = "discrete(0, 999999)", default = 0)]
    pub seed: IntParam,

    #[param(
        name = "Output Gain",
        range = "linear(-24, 12)",
        default = 0.0,
        unit = "dB",
        smooth = "exp(5)"
    )]
    pub gain: FloatParam,

    /// Original = the score's own length; Stretch = the whole score fitted into
    /// `length`; Cut = stop after `length`, like quitting the original early.
    #[param(name = "Length Mode")]
    pub length_mode: EnumParam<LengthMode>,

    #[param(name = "Length", range = "log(10, 3600)", default = 1200.0, unit = "s")]
    pub length: FloatParam,

    /// Not in the original: caps how far one grain can push the grain clock, so
    /// density valleys do not turn into minutes of silence.
    #[param(name = "Skip Silences", default = 0)]
    pub skip_silence: BoolParam,

    /// Start/stop with the host transport and follow its position.
    #[param(name = "Host Sync", default = 1)]
    pub host_sync: BoolParam,

    #[meter]
    pub meter_left: MeterSlot,

    #[meter]
    pub meter_right: MeterSlot,

    #[meter]
    pub meter_density: MeterSlot,

    /// The source sound, saved with the project (embedded, or by path).
    #[persist = "audio"]
    pub audio: SourceSlot,

    /// 0 = modern theme, 1 = classic (antiorp) theme.
    #[persist = "theme"]
    pub theme: RwLock<u8>,

    /// Render settings: `ExportFormat` as u8, and normalisation.
    #[persist = "export_format"]
    pub export_format: RwLock<u8>,

    #[persist = "export_normalize"]
    pub export_normalize: RwLock<bool>,

    /// The previous source, handed back by the audio thread so the UI drops it.
    #[skip]
    pub retired: RwLock<Option<Arc<AudioBufferData>>>,

    // --- Requests from the editor (counters: a change means "do it") ---
    #[skip]
    pub trigger_counter: AtomicU32,
    #[skip]
    pub play_counter: AtomicU32,
    #[skip]
    pub stop_counter: AtomicU32,
    #[skip]
    pub seek_counter: AtomicU32,
    /// Seek target in milliseconds of composition time.
    #[skip]
    pub seek_ms: AtomicU32,

    // --- Status published by the audio thread ---
    #[skip]
    pub status: EngineStatus,

    // --- Offline render ---
    #[skip]
    pub render: RenderStatus,
}

/// What the editor needs to draw the engine's state, all lock-free.
pub struct EngineStatus {
    pub seed: AtomicU32,
    pub score: AtomicU32,
    pub playing: AtomicBool,
    pub synced: AtomicBool,
    pub position_ms: AtomicU32,
    pub total_ms: AtomicU32,
    pub source_ms: AtomicU32,
    pub layers: AtomicU32,
    /// Recent grains, oldest first, as `source position (0..65535) << 16 | balance (0..255) << 8`.
    pub grains: [AtomicU32; GRAIN_TRAIL],
}

impl Default for EngineStatus {
    fn default() -> Self {
        Self {
            seed: AtomicU32::new(0),
            score: AtomicU32::new(0),
            playing: AtomicBool::new(false),
            synced: AtomicBool::new(false),
            position_ms: AtomicU32::new(0),
            total_ms: AtomicU32::new(0),
            source_ms: AtomicU32::new(0),
            layers: AtomicU32::new(0),
            grains: std::array::from_fn(|_| AtomicU32::new(0)),
        }
    }
}

impl EngineStatus {
    pub fn position_secs(&self) -> f64 {
        self.position_ms.load(Ordering::Relaxed) as f64 / 1000.0
    }
    pub fn total_secs(&self) -> f64 {
        self.total_ms.load(Ordering::Relaxed) as f64 / 1000.0
    }
}

#[derive(Default)]
pub struct RenderStatus {
    pub active: AtomicBool,
    pub cancel: AtomicBool,
    /// Progress in 1/10000.
    pub progress: AtomicU32,
    pub message: RwLock<Option<String>>,
}
