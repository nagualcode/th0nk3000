//! Headless renderer: `thonk-render <input.aif|wav> <output.aif|wav> [options]`.
//!
//! Renders a whole composition to disk at the input file's sample rate, like
//! thOnk_0+2 did, without opening a window.

use std::path::PathBuf;
use std::sync::atomic::AtomicBool;
use std::sync::Arc;
use std::time::Instant;
use thonk::aiff::{AudioBufferData, ExportFormat};
use thonk::engine::{offline_render_to_file, RenderOptions};
use thonk::score::{Length, LengthMode, ScoreType};
use truce::prelude::ParamEnum;

fn usage() -> ! {
    eprintln!(
        "usage: thonk-render <input> <output.wav|.aif> [options]

options:
  --score N          score number (see below), default 0
  --seed N           fixed seed (default: random)
  --length SECS      composition length in seconds (or m:ss)
  --mode MODE        stretch (default when --length is given) or cut
  --skip-silence     cap density-valley silences (not in the original)
  --clean            no 16-bit wrap: float sum + soft clip
  --density PCT      grain density, default 100
  --gain DB          output gain, default 0
  --format F         16 (default), 24 or 32f
  --normalize        peak-normalise to -0.3 dBFS

scores:"
    );
    for i in 0..12 {
        eprintln!("  {i:2}  {}", ScoreType::from_index(i).name());
    }
    std::process::exit(2);
}

fn parse_secs(s: &str) -> Option<f64> {
    let mut secs = 0.0;
    for part in s.split(':') {
        secs = secs * 60.0 + part.trim().parse::<f64>().ok()?;
    }
    Some(secs)
}

fn main() {
    let mut args = std::env::args().skip(1);
    let (Some(input), Some(output)) = (args.next(), args.next()) else {
        usage()
    };
    let mut opts = RenderOptions::default();
    let mut length_secs = None;
    let mut mode = LengthMode::Stretch;
    while let Some(a) = args.next() {
        let mut val = || args.next().unwrap_or_else(|| usage());
        match a.as_str() {
            "--score" => {
                opts.score = ScoreType::from_index(val().parse().unwrap_or_else(|_| usage()))
            }
            "--seed" => opts.seed = Some(val().parse().unwrap_or_else(|_| usage())),
            "--length" => length_secs = Some(parse_secs(&val()).unwrap_or_else(|| usage())),
            "--mode" => {
                mode = match val().as_str() {
                    "stretch" => LengthMode::Stretch,
                    "cut" => LengthMode::Cut,
                    _ => usage(),
                }
            }
            "--skip-silence" => opts.skip_silence = true,
            "--clean" => opts.wrapping = false,
            "--density" => {
                opts.density = val().parse::<f32>().unwrap_or_else(|_| usage()) / 100.0
            }
            "--gain" => opts.gain_db = val().parse().unwrap_or_else(|_| usage()),
            "--format" => {
                opts.format = match val().as_str() {
                    "16" => ExportFormat::Int16,
                    "24" => ExportFormat::Int24,
                    "32f" | "32" => ExportFormat::Float32,
                    _ => usage(),
                }
            }
            "--normalize" => opts.normalize = true,
            _ => usage(),
        }
    }
    if let Some(secs) = length_secs {
        opts.length = Length { mode, secs };
    }

    let source = AudioBufferData::load_file(&PathBuf::from(&input)).unwrap_or_else(|e| {
        eprintln!("{e}");
        std::process::exit(1)
    });
    let sr = source.sample_rate;
    let started = Instant::now();
    let out = PathBuf::from(&output);
    let result = offline_render_to_file(
        Arc::new(source),
        &opts,
        &out,
        &AtomicBool::new(false),
        |_, msg| eprint!("\r{msg}          "),
    );
    match result {
        Ok(seed) => {
            let secs = started.elapsed().as_secs_f64();
            let len = AudioBufferData::load_file(&out)
                .map(|b| b.pcm16.len() as f64 / sr)
                .unwrap_or(0.0);
            eprintln!(
                "\nwrote {output} ({len:.1} s) in {secs:.2} s ({:.0}x real time), seed {seed}",
                len / secs
            );
        }
        Err(e) => {
            eprintln!("\n{e}");
            std::process::exit(1);
        }
    }
}
