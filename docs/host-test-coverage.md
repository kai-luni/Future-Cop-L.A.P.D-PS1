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

## Adding coverage

The script reads each `tests/host/*_test.sh` for its `for unit in ...; do` list,
so a new harness only has to follow that pattern and be built once for the
measurement to pick it up.
