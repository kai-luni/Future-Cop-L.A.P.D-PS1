# Leaf suites (category A)

`leaf_a1_harness.c` and `leaf_a2_harness.c` exercise single-object ("leaf")
functions from `src/game/` under qemu-mipsel, unchanged and compiled with the
same gcc 2.7.2 + maspsx pipeline as every other host test. This does not prove
that every possible retail execution path is hardware-free.

| file | role |
| --- | --- |
| `leaf_kit.h` | per-function attribution on top of `fc_testkit.h`: `LK_FUNC("func_X")` groups checks, `lk_done()` prints `LEAF func_X checks=N failures=M` |
| `leaf_vectors.py` | independent expected values (PSn00bSDK/Psy-Q formulas, psx-spx layouts, isqrt, Park-Miller); regenerated into the build dir on every run |
| `leaf_a1_harness.c` | 76 functions without globals |
| `leaf_a2_harness.c` | 49 functions with a few globals; the harness defines every `D_` label and sets it before each check (no reliance on zero or retail init) |
| `leaf_math_harness.c` | 16 arithmetic, clamp and limit helpers (heap size accounting, RGB saturation, camera transform setters, teardown wrappers); mocks record their arguments |
| `leaf_flow_harness.c` | 18 gameplay/flow helpers (audio voice bookkeeping, volume flow, sound-event dispatch, scene-pool lifecycle), hardware-free |
| `leaf_object_harness.c` | 18 object/state helpers (stream rings, spawn descriptors, CVK sound banks, tracked voice); links `mocks/sound_mock.c` via `/* @mock sound_mock */` |
| `leaf_engage_harness.c` | 13 HUD/flow hooks and map-light pool units (panel sprite, frame-close hook, layout mode, text renderer state, glyph/string draw, script timers, light data init/release, CDCS publish, node reaper/grid update/spawner); expectations hand-worked from the decompiled bodies |
| `leaf_scene_harness.c` | 12 scene/camera/grid/map-chunk/spawn-constructor helpers (camera eye, player damage, radar markers, dome blocks, cheat dispatch, unit retirement, object grid, map chunks, GRDB init, tile pattern, kind 0x59/0x57 constructors), hardware-free |
| `leaf_core_harness.c` | 11 large SPU/GPU/scene units (voice slots, node commands, Bayer dither tables, GPU fill packets, SPU transfer/key/voice attributes, SPU pool allocator) on harness-defined `D_` fixtures; 80085F1C is `math` via `leaf_vectors.py` `bayer16`; 8008CB40 dropped (absolute kseg0 store) |
| `leaf_runtime_harness.c` | 14 hardware-free runtime units: packet and geometry builds (gouraud disc, particle emitter, spatial attenuation, map cell ray), map/material loaders (attack definition, vm registry), the heap allocator, the path mover and five small helpers (visual reset, resupply trigger, priority voice allocator, status bar, vibration toggle); expectations hand-worked from the decompiled bodies and the arithmetic they implement |
| `leaf_a*_test.sh` | **generated**; do not edit |
| `tools/host_leaf_suite.py` | scaffold / gen-script / report / mutate |

## Adding a function

1. `python3 tools/host_leaf_suite.py scaffold 8001BC44 --buildability <json> --reach <json>`
   prints the prototype copied from the unit's `__asm__` alias, the `D_`
   symbols it needs and whether it already runs elsewhere. It writes no
   expected values.
2. Paste the block into a harness, add the marker
   `/* @leaf 8001BC44 <evidence> <why> */` and write literal expectations from
   an independent source. Evidence classes: `spec`, `math`, `libc`,
   `semantic`, `source` (the last one only pins behaviour).
3. `python3 tools/host_leaf_suite.py gen-script leaf_a1` regenerates the unit
   list of the test script (literal `for unit in` line, so `host_coverage.py`
   and `host_test_reach.py` see it).
4. `python3 tools/host_leaf_suite.py report leaf_a1 --json map.json` builds,
   runs and writes the test-to-function map. A marker without checks, or an
   `LK_FUNC` block without a marker, fails the report.
5. `python3 tools/host_leaf_suite.py mutate leaf_a1 --unit 8001BC44` applies
   one mutation at a time (comparison, arithmetic, shift, mask, constant+1,
   `return 0`) to a temporary copy of the unit and of `tests/host`, and
   reports killed / survived. `src/game` is never written.

Rules: never compute an expectation by calling a re-implementation of the
unit; never depend on the initial content of a `D_` slot; replace retail
tables with synthetic fixtures whose expected output follows from the fixture.
