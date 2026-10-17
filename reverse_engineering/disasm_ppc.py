#!/usr/bin/env python3
"""
PowerPC disassembler for thOnk_0+2 with TOC-constant annotation.

Disassembles a function range from the PEF code section (loaded at 0x10000000,
file offset 0x710) and annotates every r2-relative (TOC) load with the value it
reads from the unpacked data section (loaded at 0x1000bec0, r2 == data base).

Usage:
    python3 disasm_ppc.py 0x1000842c 0x10008a78
"""

import struct
import sys

from capstone import CS_ARCH_PPC, CS_MODE_32, CS_MODE_BIG_ENDIAN, Cs

CODE_FILE_OFF = 0x710
CODE_BASE = 0x10000000
DATA_BASE = 0x1000BEC0  # == r2 (TOC); Ghidra _DAT_ addresses minus this give data offsets

APP = "original_binaries/thOnk_0+2.app"
DATA = "unpacked_data_sec.bin"


def annotate(op_str, mnem, data):
    if "(r2)" not in op_str:
        return ""
    off = int(op_str.split(",")[-1].strip().split("(")[0], 0)
    if off < 0 or off + 8 > len(data):
        return ""
    raw = data[off:off + 8]
    if mnem.startswith("lfd"):
        return f"  ; TOC+{off:#x} = {struct.unpack('>d', raw)[0]!r}"
    if mnem.startswith("lfs"):
        return f"  ; TOC+{off:#x} = {struct.unpack('>f', raw[:4])[0]!r}"
    word = struct.unpack(">I", raw[:4])[0]
    return f"  ; TOC+{off:#x} = {word:#010x}"


def main():
    start, end = (int(a, 0) for a in sys.argv[1:3])
    code = open(APP, "rb").read()
    data = open(DATA, "rb").read()
    blob = code[CODE_FILE_OFF + start - CODE_BASE: CODE_FILE_OFF + end - CODE_BASE]
    md = Cs(CS_ARCH_PPC, CS_MODE_32 | CS_MODE_BIG_ENDIAN)
    md.skipdata = True  # capstone cannot decode a few classic-PPC encodings
    for ins in md.disasm(blob, start):
        print(f"{ins.address:08x}  {ins.mnemonic:8s} {ins.op_str}{annotate(ins.op_str, ins.mnemonic, data)}")


if __name__ == "__main__":
    main()
