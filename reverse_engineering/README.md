# Reverse engineering thOnk_0+2

This folder documents how the synthesis of **thOnk_0+2** (Arjen van der Schoot /
Audio Ease, 1996–1999, UI by =cw4t7abs) was recovered from the original Mac OS
PowerPC executable. Everything that `src/score.rs` and `src/engine.rs` implement
can be traced back to an address listed here.

| File | What it is |
|---|---|
| `original_binaries/thOnk_0+2.app` | Data fork of the original app: a PEF (`Joy!peffpwpc`) container |
| `original_binaries/thOnk-resource-fork.bin` | Its resource fork (dialogs, menus, PICTs, strings) |
| `thOnk_0+2 ReadMe:Manual.txt` | The original manual |
| `pefloader.java`, `sectionheader.java` | Ghidra scripts used to load the PEF |
| `thOnk_0+2.c` | Full Ghidra decompilation (symbols survived: the binary has `.debug::` names) |
| `unpack_data_sec.py` | Decompressor for the pattern-initialised data section |
| `unpacked_data_sec.bin` | The unpacked data section (TOC, constants, strings, globals) |
| `disasm_ppc.py` | Capstone-based PowerPC disassembler that annotates every TOC load with its value |

## 1. The container

The data fork is a Code Fragment Manager PEF container with three sections:

| # | Kind | File offset | Size | Loaded at |
|---|---|---|---|---|
| 0 | code | `0x710` | `0xbec0` | `0x10000000` |
| 1 | pattern-initialised data | `0xc5d0` | `0xf05` packed → `0x1d88` | `0x1000bec0` |
| 2 | loader | `0x80` | `0x68c` | — |

`unpack_data_sec.py` expands section 1 (PEF "pidata" opcodes: zero, block,
repeat, repeat-block, repeat-zero) into `unpacked_data_sec.bin`.

## 2. The key that unlocks the decompilation: addressing

The decompiler output refers to constants two different ways, and only one of them
is right as printed:

* **TOC-relative loads** such as `*(double *)(local_10c + 0x770)`. On PowerPC
  register `r2` holds the TOC pointer, and for this fragment **r2 = data section
  base**. So `local_10c + 0x770` is simply offset `0x770` of
  `unpacked_data_sec.bin` (= 75.0).
* **Absolute `_DAT_1000cXXX` symbols.** Ghidra's custom loader placed the data
  section at `0x1000c000`, but the real base is `0x1000bec0`. Every `_DAT_` address
  must be corrected by **+0x140** to find its offset in the data file:
  `_DAT_1000c63c` → `0x77c` = 1200.0 (score length), `_DAT_1000c6a0` → `0x7e0` =
  0.01 (scheduler step), `_DAT_1000c6b0` → `0x7f0` = 3.1415926536 (envelopes),
  `PTR_LAB_1000cd64` → `0xea4` (the pentatonic table copied by `PentaScore`).

Without this correction most constants in the decompilation look like garbage
pointers; with it, every one of them resolves to a sensible value.

## 3. Where Ghidra was not enough: PowerPC calling convention

`SetFM` takes four pitch doubles, a time double and four 16-bit voice weights.
The decompiler drops half of the weights because of how Metrowerks passed them.
Disassembling `SetFM` (`0x100053d0`) with `disasm_ppc.py` shows:

```
f1..f4      pitches of voices 1..4 (semitones)
f5          time (seconds)
r5          weight 1          -> record[5]
r8          weight 2          -> record[6]
0x38(r1)    weight 3 (lha 0x3a after the callee's frame) -> record[7]
0x44(r1)    weight 4          -> record[8]
```

With that, every `SetFM` call site was read from the disassembly, not from the C.

```
python3 disasm_ppc.py 0x1000842c 0x10008a78   # Shepard, annotated
```

## 4. Random numbers

* `Random()` is the QuickDraw trap: Park–Miller (`seed = seed * 16807 mod 2^31-1`),
  returning the low 16 bits, 0x8000 mapped to 0.
* `FRand()` @ `0x10004a44` = `|Random()/32768 + Random()| / 32768`: a uniform value
  in [0, 1) with ~30 bits of resolution (not a triangular distribution).
* `Rand(n)` @ `0x10005004` = `(Random() & 0xffff) / (0x10000 / n) + 1`, i.e. 1..n.
* Every score starts with `GetDateTime(&qd.randSeed)`: the composition is seeded by
  the clock, so no two runs are alike. th0nk3000 keeps that (Seed = 0) but lets you
  lock a seed for recall.

## 5. The score model

`SetFirstBreakPoints` allocates 16 doubly linked lists: seven parameters, the
four-voice transposer, and eight "band-limited randomisers".

| List | Global | Meaning |
|---|---|---|
| GF | `d6a4` | granular frequency, grains/s — **exponential** interpolation (`FindExpParm`) |
| GL | `d6b8` | grain length in samples |
| A / D | `d6b4` / `d6b0` | attack / decay as a fraction of the grain |
| G | `d6ac` | grain gain |
| SO | `d6a8` | source offset, seconds |
| B | `d6a0` | balance; `SetB` stores `1 + value`, so 0 = left, 2 = right |
| FM | `d69c` | four pitches + four weights per node |
| GLRnd … BRnd, FMRnd | `d698`…`d67c` | width of a uniform randomiser: `v + r·FRand() − r/2` |
| **GFRnd** | `d680` | **not** a rate randomiser: it is a grain-onset jitter **in output samples** |

`SetDefaultValues` adds a breakpoint to every list at **t = 1e9 s** (GF 500, GL 500,
A/D 0.01, G 1.0, FM voice 1 at 0 st, SO 0, B centre). Values therefore drift very
slightly toward these after the last breakpoint of a score, as in the original.

`PutBPInList` inserts a node *before* nodes with equal time; `FindLinParm` returns
the head value before the first node and interpolates linearly from the last node
whose time is < t. A zero time difference is replaced by 0.01. `FindExpParm`
replaces zero values by 1e-4 before taking logarithms.

`FindFrequencyMultiplier` interpolates the pitches and (as 16-bit integers) the
weights, draws `r = (short)(100·FRand())` and picks voice 1 if `r < w1`, 2 if
`r < w1+w2`, 3 if `r < w1+w2+w3`, else voice 4. **Voice 4 gets the remainder of
100 %**: truncation of `p` and `100 − p` sometimes leaves 1 %, so in the Classic
scores a grain occasionally plays at its original pitch. The result is
`1.05946309434^(pitch + FMRnd·FRand() − FMRnd/2)`.

## 6. The renderer

### Scheduler — `BreakPointsToSoundFile` @ 0x10001abc

```
acc = 0; grain_t = start
for t = start; t <= end; t += 0.01:
    jitter = FindLinParm(t, GFRnd)
    gf = |FindExpParm(t, GF)|;  if gf == 0: gf = 1e-5
    acc += gf * 0.01
    n = ceil(acc)
    if n >= 1:
        repeat n times:
            PutGrain(grain_t, jitter·FRand() − jitter/2)
            grain_t += 1/gf
        acc -= n
```

There are two clocks. `t` drives density; `grain_t`, which advances by `1/gf` per
grain, decides *where* the grain lands and *which* parameter values it reads. When
the density falls into a deep valley (GF breakpoints of value 0 are common: 1 in 75),
a single grain can push `grain_t` forward by many seconds. This is why some
renders contain long silences and run longer than 20 minutes. It is the original
behaviour and th0nk3000 keeps it.

There is **no limit on simultaneous grains**. "600 layers" in the manual is the
theoretical maximum (6000 grains/s × 0.1 s), not a cap.

### One grain — `PutGrain` @ 0x10001cf0 → `GrainToDisk` @ 0x10004000

1. Evaluate, in this order: GL, A, D, G, FM, B, SO (each with its randomiser).
2. Clamp: length 0..441000, A and D 0..1, G 0..10000, FM 1e-5..10000, B 0..2;
   source offset to `[0, total − length·FM]` (grains never read past the end).
3. `GetGrain` @ 0x1000445c reads `length·FM` input samples and resamples them by
   linear interpolation, truncating every result to 16 bits.
4. `ShapeGrain2` @ 0x1000464c: if `A + D > 1` then `D = 1 − A`. The attack and decay
   use 1000-entry cosine tables built by `MakeCosineAttack`/`MakeCosineDecay`
   (`16384·(1 − cos(πi/1000))`, `16383·cos(πi/1000) + 16384`). Each sample becomes
   `(short)(s · G · env/32767)`; the sustain is `(short)(s · G)`.
5. Constant-power pan: left = `sqrt(1 − B/2)`, right = `sqrt(B/2)`, truncated to 16 bits.
6. `AIFFSndAddStereo` @ 0x100005d4 reads the samples already in the output file at
   the grain's position, adds the grain **as `short + short`** and writes them back.

Step 6 is the famous "thOnk wraps. Compared to which clipping is a picnic." The
wrap happens at every addition, so the result is the sum modulo 2^16.
th0nk3000 sums grains in an i32 ring and casts to i16 when reading. Modular addition
is associative, so this is **bit-identical** to wrapping at every add, while each
grain is still quantised to 16 bits exactly as in the original.

The output file has the **input's sample rate**; grain lengths and offsets are in
input samples. th0nk3000 synthesises in the source's rate as well and resamples
only at playback (4-point Hermite) when the host rate differs.

## 7. The scores

All values below were read from the TOC through `disasm_ppc.py`. "×(n+1)" means
the loop bound is inclusive (`<=`).

### Classic family: Hectic, Flowing, Flash, Slack, Dis

These five are the same function compiled five times with different constants.

Loop 1, ×(n1+1), every time drawn as `1200·FRand()`:
* **two** GF points `x² `, `x = (short)(75·FRand())` (0…5476 grains/s, 1.3 % zeros)
* GFRnd `500·r`, GL `(int)(4000·r)`, GLRnd `500·r`, B `3r − 1.5`, BRnd `6r`

Loop 2, ×(n2+1):
* SO `(src − 0.05)·r`, SORnd `k²·r/1000` with `k = (short)(20·r + 0.001)`
* FM node, two voices, weights `p` and `100 − p`
* FMRnd `3r`

Tail: GF value at t=0, SO 0.1, GL at t=0, A = D = 0.1, gain at **t = 15 s**.

| Score | n1 / n2 | weight p | voice 1 | voice 2 | GF₀ | GL₀ | gain |
|---|---|---|---|---|---|---|---|
| Classic Hectic | 600 / 600 | `100r` | `18r − 36` | `12r + 12` | 500 | 200 | 0.03 |
| Classic Flowing | 80 / 40 | `100r` | `18r − 36` | `12r + 12` | 500 | 200 | 0.03 |
| Flash | 2400 / 2400 | `100r·r`, odd → `100 − p` | `18r − 36` | `12r` | 500 | 200 | 0.03 |
| Slack | 40 / 20 | `100r·r`, odd → `100 − p` | `18r − 24` | `12r + 12` | 500 | 200 | 0.03 |
| Dis | 133 / 66 | `100r` | `18r − 52` | `20r − 12` | 1000 | 2000 | 0.02 |

(The counts come from `1200 × 0.5`, `1200 / 15` with a halving flag at
`TOC+0x178c`, `2.0 × 1200`, `1200 / 30` with the flag, and `1200 / 9`.)

### Pentathon — `PentaScore` @ 0x10008a78

GF 2000, GFRnd 100, GL 2000, A = D = 0.2, gain 0.05, BRnd 6, SORnd 0.1.
A table of 23 pentatonic steps (data `0xeac`):
`-30 -28 -26 -23 -21 -18 -16 -14 -11 -9 -6 -4 -2 1 3 6 8 10 13 15 18 20 22`.

* Upper voice index starts at `Rand(10)+5`, moves `Rand(5)−3` (−2…+2) **every step**
  and is re-drawn as `Rand(15)+5` if it leaves 6…19.
* Lower voice index starts at `Rand(5)+3`, moves **every third step** (countdown
  starting at 4), re-drawn as `Rand(10)` outside 0…12. Each move also drops a
  random SO breakpoint.
* Note times: `t += 2.5 + 16·FRand()`. The old pair sits at `t`, the new pair at
  `t + 2.5`: a 2.5 s glide, then a hold of 0–16 s. Weights 50/50, 151 steps.

### Shepard — `Shepard` @ 0x1000842c

GF 2000, GFRnd 100, GL 2000, A = D = 0.1, **gain 0.1**, FMRnd 1, BRnd 8,
SO from 0.2 s to `src − 0.2` across the 20 minutes (a slow scan), SORnd 0.1.
Opening nodes at 0, 5 and 10 s, then a loop starting at t = 15 with
`low = −28`, `high = 18`, `mid = −4`, `speed = 1.3`. Each pass writes eight nodes in
pairs 0.1 s apart. In each pair one voice jumps up while its weight is 0 (inaudible),
then fades in at `high + Rand(6)` and glides down into the `low + Rand(8)` region.
Weights rotate through (0,30,30,40), (30,0,30,40), (30,30,0,40), (35,35,30,0).
Between pairs `t += speed·(Rand(3)+3)`. After each pass `low` and `high` drop
1.5 semitones and `speed` grows by 0.3 s. That is the description in the binary:
*"keeps going down in micro and macro pitch and in speed."*

### Octaves stretch — `stretch1Score` / `stretch5Score`

Length 60 / 300 s. GF 3000, GFRnd 100, GL 3000, GLRnd 100, A = D = 0.05, gain 0.05,
SO ramps 0 → source length, BRnd 5.0 (1 min) / 7.0 (5 min).
`(int)(length × 0.125)` FM nodes at random times, pitches **0, −24, +12, 0**, weights
`40, w, 50 − w` with `w = Rand(30)`, mirrored to `60 − w` when odd. Voice 4 (0 st)
takes the remaining 10 %.

### Plain stretch — `plainStretch1` / `plainStretch5`

GF 2500, GFRnd 100, GL 2000, GLRnd 100, A = D = 0.05, gain 0.07, a single FM node
at 0 st, SO ramps 0 → source length, **BRnd 7.0** (*"only widens the image"*).

### ztochazt!k = akadem!k wank

`GoGoGranes` draws `Random() mod 16` and redraws on the menu separators (items 3, 7,
10, 13, 17), so every real score is equally likely.

## 8. Strings

The score descriptions shown in th0nk3000 are the original ones, taken from the
data section (offset ~`0x910`), together with the menu names
(*Thonk Flowing, Thonk Hectic, Pentathon, Shepard, Oct. stretch to 1, …*).

## 9. What th0nk3000 changes on purpose

| Original | th0nk3000 | Why |
|---|---|---|
| Offline, to disk only | Real-time audition plus offline render | the point of a plugin |
| Seed = clock | Seed = clock by default, optional locked seed | DAW recall |
| Stops after one score | Audition chains a new composition | continuous playback |
| Grain placement can go back in time freely | Up to 1 s back is exact; beyond that, re-anchored to "now" | a stream cannot rewrite the past (offline renders use the same rule, and it almost never triggers) |
| 16-bit mono AIFF input | WAV/AIFF, 8/16/24/32-bit, any channel count (mixed to mono, quantised to 16 bits) | convenience; the synthesis still sees 16-bit samples |
| Output only wraps | Optional "clean" mode (no wrap, tanh soft clip), density and output gain | modern options; the defaults (wrap on, 100 %, 0 dB) are the original |
| — | Live-input mode (granulates the last 5 s of input) | new |

## 10. Reproducing

```bash
cd reverse_engineering
python3 unpack_data_sec.py                     # -> unpacked_data_sec.bin
pip install capstone
python3 disasm_ppc.py 0x10001abc 0x10001cf0    # BreakPointsToSoundFile
python3 disasm_ppc.py 0x10006f30 0x10007448    # ClassicScoreHectic
```

The function addresses are listed in `thOnk_0+2.c` (`/* Função: .Name | Endereço: … */`).
The regression tests in `src/score.rs` and `src/engine.rs` check the decoded
structure: point counts, ranges, weights, envelope tables, the wrap equivalence
and the RNG.
