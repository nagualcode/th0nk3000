//! Plugin parameters declaration.

use crate::aiff::AudioBufferData;
use crate::score::ScoreType;
use std::sync::atomic::AtomicU32;
use std::sync::RwLock;
use truce::prelude::*;

#[derive(Params)]
pub struct ThonkParams {
    #[param(name = "Method")]
    pub score: EnumParam<ScoreType>,

    #[param(name = "16-Bit Wrap Clip", default = 1)]
    pub wrapping_distortion: BoolParam,

    #[param(name = "Live Audio Input", default = 0)]
    pub live_input: BoolParam,

    #[param(
        name = "Density",
        range = "linear(20, 300)",
        default = 100.0,
        unit = "%"
    )]
    pub density: FloatParam,

    #[param(
        name = "Output Gain",
        range = "linear(-24, 12)",
        default = 0.0,
        unit = "dB",
        smooth = "exp(5)"
    )]
    pub gain: FloatParam,

    #[meter]
    pub meter_left: MeterSlot,

    #[meter]
    pub meter_right: MeterSlot,

    #[meter]
    pub meter_density: MeterSlot,

    #[persist = "loaded_file"]
    pub loaded_file_name: RwLock<String>,

    #[skip]
    pub pending_file: RwLock<Option<AudioBufferData>>,

    #[skip]
    pub current_audio: RwLock<Option<AudioBufferData>>,

    #[skip]
    pub render_status: RwLock<Option<String>>,

    #[skip]
    pub trigger_counter: AtomicU32,
}
