#!/usr/bin/env python3
"""
PEF PIData (Pattern-Initialized Data) Section 1 Unpacker for thOnk_0+2.
Extracts Table of Contents (TOC) constants from offset 0xc5d0 in the PowerPC PEF binary.
"""

import sys
import struct

def unpack_pidata(f, offset, unpacked_size):
    f.seek(offset)
    out = bytearray()
    while len(out) < unpacked_size:
        b = f.read(1)
        if not b:
            break
        opcode = b[0]
        op = opcode >> 5
        count = opcode & 0x1F

        if op == 0:  # Zero
            if count == 0:
                count = struct.unpack(">I", f.read(4))[0]
            out.extend(b"\x00" * count)
        elif op == 1:  # Block
            if count == 0:
                count = struct.unpack(">I", f.read(4))[0]
            out.extend(f.read(count))
        elif op == 2:  # Repeat
            if count == 0:
                count = struct.unpack(">I", f.read(4))[0]
            rep = struct.unpack(">I", f.read(4))[0]
            chunk = f.read(count)
            for _ in range(rep):
                out.extend(chunk)
        elif op == 3:  # RepeatBlock
            if count == 0:
                count = struct.unpack(">I", f.read(4))[0]
            rep = struct.unpack(">I", f.read(4))[0]
            chunk = f.read(count)
            for _ in range(rep):
                out.extend(chunk)
        elif op == 4:  # RepeatZero
            if count == 0:
                count = struct.unpack(">I", f.read(4))[0]
            out.extend(b"\x00" * count)
        else:
            break
    return out[:unpacked_size]

if __name__ == "__main__":
    src = "original_binaries/thOnk_0+2.app"
    dst = "unpacked_data_sec.bin"
    with open(src, "rb") as f:
        data = unpack_pidata(f, 0xc5d0, 0x1d88)
        with open(dst, "wb") as out_f:
            out_f.write(data)
    print(f"Extracted {len(data)} bytes of TOC pattern-initialized data to {dst}")
