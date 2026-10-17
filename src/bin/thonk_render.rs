//! Headless renderer: `thonk-render <input.aif|wav> <output.aif|wav> [options]`.
//!
//! Renders a whole score to disk at the input file's sample rate, like
//! thOnk_0+2 did, without opening a window.

use std::path::PathBuf;
use std::time::Instant;
use thonk::aiff::AudioBufferData;
use thonk::engine::offline_render_to_file;
use thonk::score::ScoreType;
use truce::prelude::ParamEnum;

fn usage() -> ! {
    eprintln!(
        "usage: thonk-render <input> <output.wav|.aif> [--score N] [--seed N] [--clean] \
         [--density PCT] [--gain DB]\n\nscores:"
    );
    for i in 0..12 {
        eprintln!("  {i:2}  {}", ScoreType::from_index(i).name());
    }
    std::process::exit(2);
}

fn main() {
    let mut args = std::env::args().skip(1);
    let (Some(input), Some(output)) = (args.next(), args.next()) else {
        usage()
    };
    let mut score = ScoreType::ClassicFlowing;
    let mut seed = None;
    let mut wrapping = true;
    let mut density = 1.0f32;
    let mut gain = 0.0f32;
    while let Some(a) = args.next() {
        let mut val = || args.next().unwrap_or_else(|| usage());
        match a.as_str() {
            "--score" => score = ScoreType::from_index(val().parse().unwrap_or_else(|_| usage())),
            "--seed" => seed = Some(val().parse().unwrap_or_else(|_| usage())),
            "--clean" => wrapping = false,
            "--density" => density = val().parse::<f32>().unwrap_or_else(|_| usage()) / 100.0,
            "--gain" => gain = val().parse().unwrap_or_else(|_| usage()),
            _ => usage(),
        }
    }

    let source = AudioBufferData::load_file(&PathBuf::from(&input)).unwrap_or_else(|e| {
        eprintln!("{e}");
        std::process::exit(1)
    });
    let sr = source.sample_rate;
    let started = Instant::now();
    let result = offline_render_to_file(
        source,
        score,
        seed,
        wrapping,
        density,
        gain,
        &PathBuf::from(&output),
        |_, msg| eprint!("\r{msg}          "),
    );
    match result {
        Ok(seed) => {
            let secs = started.elapsed().as_secs_f64();
            let reader_len = thonk::aiff::AudioBufferData::load_file(&PathBuf::from(&output))
                .map(|b| b.samples.len() as f64 / sr)
                .unwrap_or(0.0);
            eprintln!(
                "\nwrote {output} ({reader_len:.1} s) in {secs:.2} s ({:.0}x real time), seed {seed}",
                reader_len / secs
            );
        }
        Err(e) => {
            eprintln!("\n{e}");
            std::process::exit(1);
        }
    }
}
