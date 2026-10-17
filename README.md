# th0nk3000

```
  _   _     ___         _     _____  ___   ___   ___  
 | |_| |__ / _ \ _ _   | |__ |__ / |/ _ \ / _ \ / _ \ 
 |  _| '  \ (_) | ' \  | / /  |_ \ | (_) | (_) | (_) |
  \__|_||_|\___/|_||_| |_\_\ |___/_|\___/ \___/ \___/ 
                                                       
  COLLIDER • REVERSE-ENGINEERED FROM AUDIO EASE 1996 • macOS / CLAP / VST3 / AU
```

> *"Luck is not the hand of God. Luck is the way the wind swirls, and the dust settles, eons after God has passed by."*  
> — Kurt Vonnegut (*The Sirens of Titan*), quoted in the original thOnk_0+2 manual.

A modern recreation of the cult Mac OS freeware sound generator **thOnk_0+2**
(1996–1999, **Arjen van der Schoot** / Audio Ease, interface by **=cw4t7abs** / antiorp),
rebuilt from its PowerPC executable.

It is written in Rust with the [Truce Audio Framework](https://github.com/truce-audio/truce)
and ships as a **standalone app**, an **audio plugin (AU v2, VST3, CLAP)** and a
**command-line renderer** for macOS (Apple Silicon and Intel).

---

## ⚡ Background

thOnk_0+2 had no knobs. You picked a 16-bit mono AIFF, picked a *score*, hit
**Thonk**, and left the Mac rendering a 20-minute granular "mini-composition" to
disk, often for hours. *"…where you have NO CONTROL WHATSOEVER over the process."*

th0nk3000 runs the same algorithm in real time inside a DAW, and also renders to
disk like the original did.

---

## 🔬 What was recovered

The whole synthesis path and all 11 scores were recovered from the binary:
decompiled with Ghidra, then checked instruction by instruction with a PowerPC
disassembler wherever the decompiler lost information. The full write-up, with
addresses and constants, is in
**[`reverse_engineering/README.md`](reverse_engineering/README.md)**. Highlights:

- **The 16-bit wrap.** Every grain is mixed into the output with a `short + short`
  read-add-write (`AIFFSndAddStereo`), so overloads wrap around instead of
  clipping: *"Compared to which clipping is a picnic."* th0nk3000 sums grains in
  32 bits and casts to 16 bits on output. That is bit-identical, because addition
  modulo 2¹⁶ is associative.
- **Whole-grain rendering.** Linear-interpolated resampling truncated to 16 bits,
  1000-point cosine attack/decay tables, constant-power pan, and source reads that
  never run past the end of the file, all as in the original.
- **Two clocks.** Density is evaluated every 10 ms, but each grain advances its
  own clock by `1/density`. A deep density valley can push the next grain many
  seconds ahead, which is why some renders contain long silences.
- **The four-voice statistical transposer.** Interpolated pitches and weights;
  voice 4 takes whatever percentage is left.
- **Exact score constants.** For example, "Classic Flowing" is literally
  "Classic Hectic" with 80/40 breakpoints instead of 600/600. "Shepard" fades voices
  in high and glides them down, slowing a little more every cycle. The octave
  stretches mix 0, −24 and +12 semitones.

### The scores

| Score | Length | Character |
|---|---|---|
| Classic Flowing | 20 min | the original Thonk, sparse: slowly evolving narrative |
| Classic Hectic | 20 min | the original Thonk, dense: fast moving events |
| Pentathon | 20 min | two-voice Brownian pentatonic counterpoint (23-step table) |
| Shepard | 20 min | endlessly descending, slowing circular pitch |
| Dis | 20 min | dark, 3–4 octaves down, everything moving |
| Octaves Stretch 1 / 5 min | 1 / 5 min | input stretched, octaves fade in and out |
| Plain Stretch 1 / 5 min | 1 / 5 min | input stretched, widened stereo image, no transposition |
| Flash | 20 min | Hectic with 4× the breakpoints |
| Slack | 20 min | Hectic with 40 breakpoints, low voice an octave higher |
| ztochazt!k = akadem!k wank | — | one of the above at random |

---

## 🚀 Features

- **Authentic engine.** The defaults (wrap on, density 100 %, gain 0 dB) reproduce
  the original algorithm. Offline renders run at the input file's sample rate,
  like the original, and contain the exact 16-bit sums it would have written.
- **Seeds.** Seed 0 gives a new composition on every Thonk (the original seeded
  from the clock). **LOCK SEED** stores the composition you are hearing so a DAW
  project recalls it.
- **Real-time audition** in the standalone app or on a DAW track, and **render to
  disk** as 16-bit stereo **WAV or AIFF**.
- **Modern extras:** clean mode (no wrap, soft clip), density multiplier, output
  gain, live-input mode (granulates the last 5 s of the track or mic).
- Loads WAV and AIFF (8/16/24/32-bit, any channel count) at any sample rate.
  The engine synthesises at the file's rate and resamples only on output.
- Real-time safe: no allocation on the audio thread, and grains are mixed whole
  ahead of playback. Roughly 2 ns per grain sample, so a dense 20-minute score
  renders offline at about 110–260× real time on Apple Silicon.

---

## 🛠️ Install

Download from the [Releases](https://github.com/nagualcode/th0nk3000/releases) page:
the `.pkg` installer, or the `.zip` to copy the bundles by hand. The binaries are
ad-hoc signed, not notarised. If macOS blocks them, right-click → Open, or run
`xattr -dr com.apple.quarantine <bundle>`.

## 🧱 Build

Requirements: macOS 12+, Rust 1.85+.

```bash
git clone https://github.com/nagualcode/th0nk3000.git
cd th0nk3000

cargo test --release                                  # engine + score regression tests
cargo build --release --bin thonk-standalone
cargo build --release --bin thonk-render              # headless renderer

# installers (requires cargo-truce)
cargo truce package --universal --no-notarize --formats clap,vst3,au2,standalone
```

### Headless rendering

```bash
./target/release/thonk-render input.aif output.aif --score 1 --seed 1995
# options: --score N (0-11, run without arguments for the list)  --seed N
#          --clean  --density PCT  --gain DB
```

---

## 📁 Repository

```
th0nk3000/
├── src/
│   ├── lib.rs          # plugin entry point (Truce PluginLogic)
│   ├── engine.rs       # scheduler, whole-grain renderer, 16-bit wrap, ring buffer
│   ├── score.rs        # the 11 scores, breakpoint lists, QuickDraw RNG
│   ├── aiff.rs         # WAV/AIFF loading, 16-bit WAV/AIFF writer
│   ├── params.rs       # parameters (score, wrap, live, density, seed, gain)
│   ├── ui.rs           # egui interface
│   ├── main.rs         # standalone app
│   └── bin/thonk_render.rs
└── reverse_engineering/
    ├── README.md       # the reverse-engineering write-up
    ├── thOnk_0+2.c     # Ghidra decompilation
    ├── disasm_ppc.py   # PowerPC disassembler with TOC annotation
    ├── unpack_data_sec.py / unpacked_data_sec.bin
    ├── pefloader.java / sectionheader.java
    ├── thOnk_0+2 ReadMe:Manual.txt
    └── original_binaries/
```

---

## 📜 License & Credits

- Modern implementation: **MIT License** © 2026 nagualcode / Jardim Sonoro.
- Original thOnk concept and algorithms: © 1996–1999 **Arjen van der Schoot** (Audio Ease), with early help from Peter Bakker.
- Original visual design: **=cw4t7abs** (antiorp).
- Audio plugin framework: [Truce Audio](https://github.com/truce-audio/truce).
