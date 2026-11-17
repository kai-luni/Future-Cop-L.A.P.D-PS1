#!/usr/bin/env python3
"""Re-insert alignment for data labels after maspsx.

maspsx expands .lcomm/.comm into `label:` + `.space N` and drops the alignment
gcc emitted, leaving u32 objects at odd addresses (SIGBUS on MIPS). Insert
`.align 2` before any standalone label whose next directive is data storage.
Text labels (followed by instructions) are left untouched.
"""
import re
import sys

LABEL = re.compile(r"^([A-Za-z_.][A-Za-z0-9_.]*):\s*$")
DATA = re.compile(r"^\s*\.(space|word|half|byte|long|short|quad|dword|float|double|ascii|asciz)\b")

lines = sys.stdin.read().splitlines()
out = []
for i, line in enumerate(lines):
    m = LABEL.match(line)
    if m:
        j = i + 1
        while j < len(lines) and not lines[j].strip():
            j += 1
        if j < len(lines) and DATA.match(lines[j]):
            out.append("\t.align\t2")
    out.append(line)
sys.stdout.write("\n".join(out) + "\n")