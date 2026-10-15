# th0nk3000

```
  _   _     ___         _     _____  ___   ___   ___  
 | |_| |__ / _ \ _ _   | |__ |__ / |/ _ \ / _ \ / _ \ 
 |  _| '  \ (_) | ' \  | / /  |_ \ | (_) | (_) | (_) |
  \__|_||_|\___/|_||_| |_\_\ |___/_|\___/ \___/ \___/ 
                                                       
  COLLIDER • REVERSE-ENGINEERED FROM AUDIO EASE 1995 • macOS / CLAP / VST3 / AU
```

> *"Luck is not the hand of God. Luck is the way the wind swirls, and the dust settles, eons after God has passed by."*  
> — Kurt Vonnegut (*The Sirens of Titan*), quoted in the original Th0nk 0+2 manual.

A bit-accurate modern reverse-engineering and recreation of the cult classic Mac OS 9 public domain sound transformation tool **thOnk_0+2** (created in 1995–1997 by **Arjen van der Schoot** / Audio Ease, with brutalist interface by **=cw4t7abs** / antiorp).

Re-implemented in Rust using the [Truce Audio Framework](https://github.com/truce-audio/truce) as both a **standalone desktop application** and an **audio plugin (Audio Unit v2, VST3, CLAP)** for modern macOS (Apple Silicon & Intel).

---

## ⚡ Background & History

In the mid-1990s, Arjen van der Schoot (co-founder of Audio Ease) released **Th0nk**, an uncompromising artistic application for Mac OS 7–9. Unlike commercial audio plugins, Th0nk removed conventional user parameters:
* The user selected an audio sample (originally a 16-bit mono AIFF file).
* Selected an algorithmic composition score.
* Hit **ThOnk**.
* The computer entered a dedicated calculation loop (`_BreakPointsToSoundFile`), generating an evolving 20-minute granular sound sculpture rendered directly to disk.

Because a 1995 PowerPC 601/604 (66–120 MHz) lacked the computational horsepower to calculate 600 concurrent layers in real time, Th0nk was strictly an offline disk processor. Today, **th0nk3000** brings back the complete authentic DSP architecture and adds real-time DAW auditioning and instant disk export at over **350× real-time speed** (~3.4 seconds to render a 20-minute piece on Apple Silicon).

---

## 🔬 Reverse Engineering Discoveries

Disassembled and decompiled from the original PowerPC CFM/PEF binary (`thOnk_0+2.app`) via Ghidra:

### 1. 16-Bit Integer Wrapping Distortion
When layers summed beyond 0 dBFS ($> +32767$ or $< -32768$), the PowerPC arithmetic performed a signed 16-bit integer wrap (`i16::wrapping_add`), violently inverting polarities. As the original manual notes:
> *"If thOnk_0+2 goes over 0dB Thonk wraps. Compared to which clipping is a picnic."*  
In **th0nk3000**, this signature crunch is authentically emulated and can be toggled on or off.

### 2. Quadratic Granular Frequency
Grains fire at rates modulated by squared random numbers:
```c
grain_freq = (int)(75.0 * FRand()) ^ 2; // Peaks at 5625 grains per second!
```
With grain lengths up to 100 ms, vertical density reaches up to **600 concurrent overlapping layers**.

### 3. The 23-Note Pentatonic Scale
Extracted from Table of Contents (TOC) offset `0x1000ceac` in the unpacked PEF Section 1 (PIData):
```rust
[-30, -28, -26, -23, -21, -18, -16, -14, -11, -9, -6, -4, -2, 1, 3, 6, 8, 10, 13, 15, 18, 20, 22]
```
The `Pentathon` score navigates this scale using a 2-voice Brownian random walk counterpoint.

### 4. All 12 Original Scores Modeled
1. **Classic Flowing:** Slowly evolving natural soundscapes (20 minutes).
2. **Classic Hectic:** Hyperactive spatial collisions (20 minutes).
3. **Pentathon:** 2-voice Brownian walk pentatonic counterpoint.
4. **Shepard:** 4-voice circular pitch glissando illusion (Shepard-Risset).
5. **Dis:** Subterranean pitch drops down 3 to 4 octaves (-52 to -12 semitones).
6. **Octaves Stretch (1 min):** Fits input to 1 minute while octaves fade in and out.
7. **Octaves Stretch (5 min):** 5-minute variant of octave-based stretching.
8. **Plain Stretch (1 min):** 1-minute time-stretch preserving original pitch.
9. **Plain Stretch (5 min):** 5-minute time-stretch preserving original pitch.
10. **Flash:** 2400 high-density sprint breakpoints.
11. **Slack:** 40 contemplative, sparse breakpoints.
12. **ztochazt!k = akadem!k wank (Stochastic):** Randomly mutates scores on every trigger.

---

## 🚀 Features

- **Brutalist GUI:** Cyberpunk interface inspired by =cw4t7abs / antiorp with CRT phosphor green terminal readouts, 600-layer density monitor, and stereo VU meters.
- **Dual Operating Modes:**
  - `⚡ T H O N K (AUDITION)`: Real-time auditioning through speakers/headphones or DAW track.
  - `💾 RENDER TO DISK`: Native macOS file dialog to export 16-bit stereo WAV/AIFF directly to disk at ~350× real-time.
- **Audio File Loading:** Supports 16/24-bit WAV and AIFF files (including Apple 80-bit IEEE 754 extended float sample rate decoding).
- **Live DAW / Mic Input:** Real-time rolling circular buffer to granulate incoming microphone or audio track signals.
- **Universal Formats:**
  - Standalone Application (`thonk-standalone`)
  - Audio Unit v2 (`Th0nk.component`)
  - VST3 (`Th0nk.vst3`)
  - CLAP (`Th0nk.clap`)

---

## 🛠️ Building & Installation

### Requirements
- macOS 12+ (Apple Silicon or Intel)
- Rust 1.85+ (`curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh`)

### Clone & Build
```bash
git clone https://github.com/nagualcode/th0nk3000.git
cd th0nk3000

# Build optimized standalone binary
cargo build --release --bin thonk-standalone
```

The compiled standalone executable will be located at:
```bash
./target/release/thonk-standalone
```

### Launch Standalone App
```bash
./target/release/thonk-standalone
```

### Headless CLI Rendering
Render directly from terminal without a GUI:
```bash
./target/release/thonk-standalone \
  --input-file input_sample.wav \
  --output-file rendered_output.wav \
  --no-playback
```

---

## 📁 Repository Structure

```
th0nk3000/
├── Cargo.toml                  # Cargo manifest with Truce git dependencies
├── truce.toml                  # Truce plugin descriptor (FourCC: ThNk)
├── LICENSE                     # MIT Open Source License
├── README.md                   # This documentation
├── src/
│   ├── lib.rs                  # Plugin entry point & unit tests
│   ├── main.rs                 # Standalone binary runner
│   ├── engine.rs               # 600-layer granular DSP & 16-bit wrap distortion
│   ├── score.rs                # 12 reverse-engineered scoring algorithms
│   ├── aiff.rs                 # AIFF/WAV decoder & 80-bit IEEE parser
│   ├── params.rs               # Truce plugin parameters & meter slots
│   └── ui.rs                   # Egui brutalist antiorp interface
└── reverse_engineering/
    ├── thOnk_0+2.c             # Full Ghidra decompilation of original C code
    ├── pefloader.java          # Ghidra PowerPC PEF loader script
    ├── sectionheader.java      # PEF section header analyzer
    ├── unpack_data_sec.py      # Python script to extract PIData TOC
    ├── unpacked_data_sec.bin   # Unpacked TOC constants & pentatonic scale
    ├── thOnk_0+2 ReadMe:Manual.txt # Original 1995 manual by Arjen van der Schoot
    └── original_binaries/      # Original Mac OS 9 PowerPC PEF application
```

---

## 📜 License & Credits

- Modern implementation: **MIT License** © 2026 nagualcode.
- Original Th0nk Concept & Algorithms: © 1995–1997 **Arjen van der Schoot** (Audio Ease).
- Original Visual Design: © 1996 **=cw4t7abs** (antiorp / Netochka Nezvanova).
- Audio Plugin Framework: [Truce Audio](https://github.com/truce-audio/truce).
