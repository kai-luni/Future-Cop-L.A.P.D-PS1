# Measuring host test coverage

`gcov`/`lcov` cannot be used here: the toolchain is gcc 2.7.2, which has no
`-fprofile-arcs`, and the tests are MIPS ELFs run under qemu. Two things that
*are* available are enough to measure line coverage of the code the host tests
execute:

1. `gcc 2.7.2 -g` emits `.loc` directives, so `mipsel-linux-gnu-objdump -d -l`
   maps every instruction of a unit to its source line.
2. `qemu-mipsel -d in_asm` prints every translated basic block with its guest
   addresses - i.e. every instruction that actually ran.

`tools/host_coverage.py` compiles each unit the way its test script does (same
`-O1 -G0`, plus `-g`), reads the line map from `objdump`, runs the built test
binary under qemu, and intersects both. Result: per-file covered/total lines, an
optional LCOV tracefile and an optional JSON report.

    make coverage                 # table over all host tests
    make coverage LCOV=cov.info   # plus an LCOV tracefile for any viewer
    python3 tools/host_coverage.py --test weapon_input   # one suite
    python3 tools/host_coverage.py --unit 8006BA50       # one unit

## What the numbers mean

A line counts as covered when at least one instruction attributed to it executed.
Only lines that produced code are counted, so declarations, comments and struct
definitions never enter the denominator.

The denominator is therefore *not* the whole repo: it is the set of units the
host tests compile. As of the current tree that is 53 of 1772 units (469 lines),
all at 100 %. The other units run only on the real console (they need the GTE,
the SPU, CD access or the BIOS) and are covered by the byte-exact matching check
instead - they are verified to produce identical machine code, not to behave
correctly under test.

## How many functions do the tests reach?

`make coverage` answers a narrower question than it looks: it counts lines in the
units a test script names. A test binary also links those units' real
dependencies, and different tests reach different branches. `make reach` measures
the honest number:

    make reach            # summary
    make reach LIST=1     # plus the per-function verdict

It unions the executed lines of every unit across every binary that links it, and
it distinguishes three states:

* `executed` - at least one of its source lines ran;
* `executed (handwritten asm, no line info)` - the seven GTE transcription units
  are `.word` streams with no `.loc` lines, so they are checked for execution by
  address range instead of by line;
* `linked but not executed` - present in the binary, never called.

As of the current tree: **100 decompiled functions are linked into the test
binaries and all 100 execute**, 7 of them as handwritten asm. Of the 1567 source
lines those 100 units carry, 1478 run (94.3 %). Eight units are only partly
exercised - the largest gap is `func_80044B34` at 19/72 lines.

The denominator stays the linked subset. Of the ~1763 functions in the game,
100 are reachable by a host test; the rest need GTE, SPU, CD or BIOS services and
are covered by the byte-exact matching check (1771/1771 units) instead.

## Adding coverage

The script reads each `tests/host/*_test.sh` for its `for unit in ...; do` list,
so a new harness only has to follow that pattern and be built once for the
measurement to pick it up.
