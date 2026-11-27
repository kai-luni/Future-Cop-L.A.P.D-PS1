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
| `leaf_systems_harness.c` | 14 single-object gameplay/SPU units (animation stepping, card format, entity field query, map-zone load, spawn-chain marking, anchor sync, weapon-sound pool, path-curve offsets, looping-path advance, hover vector, sound toggle, rectangle draw list, SPU common/voice attributes) plus the five GPU packet builders they call (`80093F58`/`80093F78`/`80094010`/`800940A8`/`800940C4`, `spec` for psx-spx GP0 `E1h`/`E2h` Mask+Offset/`E3h`–`E4h` with GPU-variant-dependent 10-bit Y/`E5h` and Psy-Q `libgpu.h` layouts, with C API/clamping/null-guard details marked `source`), all on synthetic fixtures; `func_80046C84`/`func_800451E0` via `/* @mock sound_mock */`; 8005AB8C dropped (three retail tables behind hard-coded kseg0 addresses) |
| `leaf_render_harness.c` | 15 render/map/sound/entity units (ray-burst POLY_G3 packets, line quad, sound storage init, type-27 entity setup, effect light spawn, path-mover commands, weapon dome effect, tile animations, map grid ray DDA, loadout difficulty tiers, dome patch mesh, music preview menu, stale node reaper, movie SpuVoiceAttr, ResetGraph); 80014490/80086C74/80094F4C are `spec` (psx-spx GP0/GP1/DMA, Psy-Q POLY_G3/SpuVoiceAttr/ResetGraph; 80094F4C `mode & 7` dispatch is `source` and RAM-backed GP1 observes only the final store), 80015830 `math` (Euclidean corner offsets), 800684AC `semantic` against exact segment/grid-line intersections; volume getters via `/* @mock sound_mock */` |
| `leaf_entity_harness.c` | 10 stream, voice, split-screen, results, and entity request units on synthetic fixtures. 80022BEC (read at `0x800A097C`) and 800247A8 (write at `0x8009FA46`) cannot run in QEMU user mode. 80029748 covers inactive and audio-only playback; active animation needs absolute kseg0 RAM at `0x800A0C30`. |
| `leaf_runtime_harness.c` | 14 hardware-free runtime units: packet and geometry builds (gouraud disc, particle emitter, spatial attenuation, map cell ray), map/material loaders (attack definition, vm registry), the heap allocator, the path mover and five small helpers (visual reset, resupply trigger, priority voice allocator, status bar, vibration toggle); expectations hand-worked from the decompiled bodies and the arithmetic they implement; 800695A8 dropped (its height-grid sampler reuses `fi`/`prod`/`off` as overlapping register-asm temporaries, so the emitted sample indices run past the 64-byte block and cannot be pinned without the fixture writing outside its buffer) |
| `leaf_gameplay_harness.c` | 15 gameplay/HUD/front-end units (player resupply, chase approach speed, light-object constructor, animation/attack script commands, HUD gouraud bar, marker colours, path-node loader, sprite descriptors, dual-view object grid, status-slot icon, particle light, options preview stop, password unlocks, atlas sprite); byte-buffer fixtures read through offset accessors, harness-defined `D_` labels, synthetic password alphabets/atlas/effect tables; 8006BF44 and 80082B0C are `spec` (Psy-Q setDrawTPage/setSprt/setPolyFT4, psx-spx GP0 layouts) |
| `leaf_lock_on_harness.c` | 3 original gameplay units: fixed-point range and facing, nearest eligible opponent, and player lock-on acquisition/cycling. Only GTE SquareRoot12 is replaced (explicit exact-root fixtures); the opponent list and player state are synthetic. `source` evidence, not validation of the retail GTE lookup table. |
| `leaf_plat_harness.c` | 30 platform-client units of the CD/SPU/GPU/MDEC library band 0x8008D..0x8009 with no prior coverage: CD read/sector/sync/ready forwarders and reset, the retail CdInit bootstrap (five retries, driver-table installation, failure print), the three fixed-value event dispatchers, callback exchange and pause-restore clients, command-name table lookup, 12-byte filename matcher, VSync dispatcher, GPU debug-level exchange with trace hook, packed tpage/clut dump dispatchers, MDEC DMA/in-sync/out-sync/option reset, SPU warm start, transfer sequence issuers, capped SPU write entry points, DRAWENV/DISPENV cache copies; every callee replaced by a harness stand-in bound to the retail label, `D_` slots owned by the harness (`D_8009DDxx` GPU state cluster via one `.data` block, `D_8009DDF2` as a same-object `.set` alias into a C array); all `source` evidence |
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

## Media and format suites

Both suites use `source` evidence: literal expectations pin the behavior of
the unchanged decompiled functions on synthetic inputs. The test scripts are
generated with `tools/host_leaf_suite.py gen-script`. Both Make targets run
from `make check`; neither silently skips missing tools.

| Make target / harness | Original entry points and tested behavior |
| --- | --- |
| `host-leaf-formats` / `leaf_formats_harness.c` | `80090218` reads mock PVD/path sectors: three early failures, odd/even record lengths, name termination, empty table, exact sector end and 128-row cap. `80086FE4` pushes mono/stereo WVE samples into two RAM rings: short/long headers, contiguous and wrapped copies, exact tail and zero payload. `80020F34` builds the selected memory-card directory frame: fixed fields, empty/20-byte names, cleared padding, XOR checksum and neighboring canaries. |
| `host-leaf-formats-graphics` / `leaf_formats_graphics_harness.c` | `8005A414` unpacks two Cpyr entries, crosses a strip row, sends the trailing atlas and delegates cleanup on an empty list, failed table allocation or descriptor-build failure. `8009446C` clamps signed dimensions, rejects empty/timed-out transfers, and writes the CPU remainder and DMA setup to RAM-backed registers. |

The ISO sector and BIOS substitutes do not model a CD drive; the card test
does not issue a write. The audio consumer is already at a known cursor (no
asynchronous playback or busy-wait test). The Cpyr allocator, descriptor
builder and cleanup are stand-ins, and the shared GPU mock records transfer
arguments without writing VRAM. The upload test observes only final register
values, not the GP0 write sequence, bus timing or DMA completion.
