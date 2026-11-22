# Host tests: adding units, using mocks, reading coverage

How to grow the host test suite without touching the decompiled sources. The
byte-identical C under `src/` is never modified for a test: a test *links* it
and supplies only what the unit cannot get on a normal desktop (hardware
registers, BIOS services, the retail data tables).

## The shape of a test

Every host test is a pair:

```
tests/host/<name>_harness.c   the test: fixtures, inputs, assertions, main()
tests/host/<name>_test.sh     compiles the units, links, runs under qemu
```

`<name>_test.sh` must be callable on its own and print a single PASS/FAIL line.
To get it into `make check`, add a Makefile target next to the existing
`host-*` ones and list it in `check:`. Keep the skip guard: when qemu or the C
toolchain is missing the target prints `skipped` and exits 0, so a fresh
checkout can still run the rest.

`tests/host/terrain_surface_test.sh` is the reference: it generates its data
tables from the retail image, compiles two real units, and asserts 50 checks.

## Using the shared kit

Include `fc_testkit.h`. It is freestanding (no libc) and provides:

| Piece | Use |
|---|---|
| `fck_write`, `fck_write_int` | output through the `write` syscall |
| `FCK_CHECK(cond, msg)` | a boolean assertion, keeps running on failure |
| `FCK_EQ(actual, expected, msg)` | equality; prints both values on failure |
| `fck_report("<suite>")` | prints the tally, returns 0 only if all held |
| `fck_record(name, a0, a1, a2)` | log a stand-in call |
| `fck_calls_to(name)`, `fck_call_n(name, nth)` | count / inspect recorded calls |
| `fck_expect_order(expected[], n, msg)` | assert the whole call order |
| `fck_log_reset()` | clear the recorder between cases |

`fck_report` fails when a suite ran *no* checks, so a harness that silently
asserts nothing cannot pass.

## Binding a stand-in to a retail symbol

Never stub a function by C name. Bind it to the exact retail label, so a wrong
name is a link error instead of a silently different function:

```c
/* Only the BIOS service this unit actually calls. */
void fckBiosPrintf(const char *fmt) __asm__("bios_A0_3F_printf");
```

Rules that keep a mock honest:

- **Signature must match the original prototype** exactly. These are 32-bit MIPS
  pointers and words; a mismatched prototype is undefined behaviour that appears
  to work until it does not. Take the prototype from `analysis/functions.tsv` or
  the unit's own declaration in `src/`.
- **Unconfigured return is 0.** Never invent a plausible value. A test that
  depends on the return must configure it first.
- **An unsupported BIOS service must report loudly**, never `return 0`
  silently — a silent zero turns a missing mock into a wrong answer.
- **Mocks are included per test**, one at a time. Never a global stub file that
  could replace real logic without anyone noticing.
- **Data globals need explicit fixtures.** A `D_` name alone says nothing about
  initialization, size, pointer targets or runtime state. The zero slots used
  by the buildability probe are only link placeholders, not execution fixtures.
  When appropriate, extract known-sized tables from the local retail image at
  `0x800 + (addr - 0x80010000)`; do not commit generated proprietary bytes.

## Choosing the next unit

```bash
python3 tools/host_test_candidates.py --top 20   # ranked mock frontier
python3 tools/host_test_candidates.py --list clean
```

The older `host_test_candidates.py` command remains useful for exploring the
frontier, but its `clean` label is not proof of hardware freedom: the analysis
has known missing call edges. For the combined object and graph assessment run:

```bash
python3 tools/host_hw_classification.py --json "$TMPDIR/host-hw.json"
```

The combined categories are `candidate` (no known hardware path or recognized
gap in the model), `hw` (a known hardware path), and `unknown` (insufficient
evidence). Even `candidate` is not proof that all hardware paths are absent.
A frontier estimate is a prioritization hint, not proof that a mock unblocks
all of the listed functions.

To check whether a chain actually builds and links on the host before writing
assertions:

```bash
python3 tools/host_test_buildability.py --unit 80067684
```

## Writing assertions that mean something

The expected value must be justified independently of the code under test.
Recording today's output and pasting it back is not a test — it only detects
drift.

Good sources of truth, in order of strength:

1. **Arithmetic from the unit's own doc block**, recomputed in Python (or by
   hand) and pasted as a literal. `terrain_surface_harness.c` does this for the
   half-plane tests and the bilinear interpolation.
2. **An invariant**: a sorted list has `min <= any <= max`; reversing the input
   order must not change a sorted result; a single-element case makes the lowest
   equal the highest.
3. **Bytes from the retail image**, for tables and constants.
4. **Cross-check against a second verified implementation**, where one exists.

Cover the boundary, not just the middle: the exact reject/accept threshold, one
step either side, and the empty/null case. A sampler that misses must leave the
output untouched — assert that too, so a "miss" cannot quietly become a height
of zero.

## Reading coverage honestly

Two different numbers, and they answer different questions:

- `make reach` — estimated execution of `func_` symbols from recorded test
  traces. A matching unit object is required, but its presence alone does not
  prove it was linked instead of a mock; see `docs/host-test-current.md` for
  this and other measurement limits. Call-chain units may also be included.
- `make coverage` — line coverage for units named in selected test scripts, from
  `qemu -d in_asm` mapped onto `.loc` info. These scripts may require fixtures;
  a failed or skipped measurement must not be treated as coverage. Translated
  blocks are close to, but not identical with, instructions actually run, so
  treat the measurement as an upper bound.

A unit that is merely *linked* is not covered. A unit that is *executed* but
never asserted on is worth much less than one with targeted checks — the suite
tally from `fck_report` is the honest per-test figure.

## When a mock is not the answer

Do not mock:

- a unit with no known hardware dependency — investigate and link real code
  where possible; the old `clean` label alone is not sufficient evidence;
- a unit whose absence is a call-graph gap — resolve the edge instead;
- an in-unit hardware access (a `mtc2` in the middle of the function body). A
  linker stub cannot replace that; the unit needs a different approach entirely
  (a GTE reference implementation, as `gte_ref.c` does).
