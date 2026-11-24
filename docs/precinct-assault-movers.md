# Precinct Assault Moving Map Objects: Lifts and Turret Carriers

## Overview

Two families of map-placed moving objects share the entity message system:
the type-10 lift sequence (dispatch → init from a script record → per-tick
stepping) and the type-20 mounted-turret carrier (a moving class-5 platform
that spawns, owns and synchronises a child part). Both are reached through
the entity-type handler lookup `func_8002AE54` (`0x8002AE54`), whose retail
form is a real compiled jump table: `(u32)(type - 1) >= 0x50` falls to the
default arm, otherwise the target comes from the word table at `0x80010E6C`
and per-case 12-byte trampolines (`lui $2,hi / j 0x8002AF8C / addiu $2,$2,lo`).
The type-10 handler address is completed at `0x8002AED0` (`lui 0x8004` +
`addiu 0x1B6C` → `0x80041B6C`, jump-table index 9), the type-20 handler at
`0x8002AF18` (`lui 0x8003` + sign-extended `addiu 0x95A8` → `0x800395A8`),
both verified from the retail bytes of `SLUS_007.39`.

The map-placed entity source itself is the `"Csac"` asset queue: the asset
loader `func_80024EC8` (`src/game/80024EC8.c`) hands every `"Csac"`
(0x43736163) request to `func_80060E74` (`src/game/80060E74.c`), which
records a spawner whose handler is selected by
`gGetEntityHandlerForType(*(u8 *)(data->unk28 + 4))` (`src/game/80060E74.c`
line 143) — i.e. the type byte in the asset record drives the same
`func_8002AE54` lookup.

## Function table

| address | retail symbol | current C name | role | size |
|---|---|---|---|---|
| `0x8002AD58` | `func_8002AD58` | `gEntityBaseHandler` | shared base message handler (default arm of every family) | 0xFC |
| `0x8002AE54` | `func_8002AE54` | `gGetEntityHandlerForType` | entity-type → handler lookup (retail jump table) | 0x140 |
| `0x80041B6C` | `func_80041B6C` | `gHandleElevatorEntityMessages` | type-10 message dispatch, hit counter | 0x100 |
| `0x80041488` | `func_80041488` | `gInitElevator` | type-10 message-2 init from the script record | 0x2A0 |
| `0x80041880` | `func_80041880` | `gStepElevator` | type-10 per-tick motion, dwell/hold gates, sounds | 0x2EC |
| `0x80041728` | `func_80041728` | `gRunEntityCommands` | four-slot command interpreter of the lift family | 0x158 |
| `0x80041C6C` | `func_80041C6C` | `gHandlePlacedPropMsg` | type-12 placed-prop handler (same script-record idiom) | 0x158 |
| `0x80041E0C` | `func_80041E0C` | `gInitPortraitScreenEntity` | portrait-screen entity init (same ground-snap idiom) | 0x1F8 |
| `0x80039504` | `func_80039504` | `gSetupMountedTurretCarrier` | carrier spawn: class-5 init + child spawn + registration | 0xA4 |
| `0x800395A8` | `func_800395A8` | `gHandleMountedTurretCarrierMessages` | type-20 dispatch wrapping class-5 + child sync | 0x204 |
| `0x80038A00` | `func_80038A00` | `gUpdateMountedTurret` | child turret update: animation, aim, yaw/pitch steering | 0x898 |
| `0x80039298` | `func_80039298` | `gHandleMountedPartMsg` | container dispatch for the spawned kind-0x15 child | 0x180 |
| `0x80039418` | `func_80039418` | `gSpawnMountedPartChild` | assembles the kind-0x15 spawn request (handler is itself) | 0xEC |
| `0x80060E74` | `func_80060E74` | `gRegisterUnitSpawner` | `"Csac"` record registration into `D_8009F800` | — |

Sizes from `analysis/symbol_addrs.txt`; every unit was read in full for this
document and the handler addresses were verified against the retail bytes.

## The lift chain (entity type 10)

Message protocol of `gHandleElevatorEntityMessages` (`src/game/80041B6C.c`,
sole entry into the family):

| message | behaviour | return |
|---|---|---|
| 1 | block size — the entity class needs `0xD0` bytes | `0xD0` |
| 2 | `gInitElevator(obj, a2, a3)` — script-record init | `0` |
| 3, 4, 7..10, 12+ (default) | `gEntityBaseHandler(msg, obj, a2, a3)` | base result |
| 5 | `gRemoveGroundKeyEntry(obj)` (func_80063194, swap-delete from the ground key table), then base handler | `0` |
| 6 | `gStepElevator(obj, a2)` — per-tick motion | step result (always 0) |
| 11 | hit counter: `if (obj->count < 20) obj->count++` on the s16 at `+0xC4`, then base handler | `0` |

### Init from the script record (`gInitElevator`, `src/game/80041488.c`)

`func_8002AD58(2, self, arg, a2)` runs first, then the record
(`struct Data` in the unit) is copied and rescaled:
`values[0..2] = pos[0..2] << 4`, `speeds[i] = (speed[i] << 8) / 218`,
`other[0..1] = (other[i] << 4) / 300`, `mode = 1`, flags/kind/extra copied.
A volatile round-trip stores `previous = old` and clears `old`, then
`last = end - 1`; if `end >= count`, `mode = 2` and `end = last - 1`.
`current = 1` when `(flags & 5) != 0`, else `speeds[last]`; `zero = 0`.
`gRegisterGroundObject(self, kind)` (func_8006314C) registers the ground
kind, `gBuildRotationMatrix(angle, 0, 0, basis)` (func_800188D0) builds the
basis from the record angle, `gGetGroundHeight(pos, 0, 0, 0)` (func_80066F78)
returns the ground height that is added to **all** stop values, and
`pos[1] = values[last]` places the lift at its start stop. The tail loads
the basis with `gGteLoadMatrix` (func_80010750) and transforms one frame
vertex with `gGteTransformVerticesWide(model->data + (index << 4) + 8,
self + 0x5C, 1)` (func_80010234).

### Per-tick stepping (`gStepElevator`, `src/game/80041880.c`)

Gate order, exactly as the code runs it:

1. `unk0C != 0` → `gRunEntityCommands(obj)` processes queued commands first.
2. `unkC6 & 2` (frozen) → skip the whole body and the tail.
3. `unkC4 > 0` → decrement once (not by dt) and skip the body and tail.
4. Player-riding flag: set when this object is the `unkC4` entity of one of
   the two `D_8009FA58` games and that game's `unkAC->unk40` is 0.
5. Advance gating: with `(unkC6 & 5) != 0` (trigger mode) the step only runs
   once `unkC0 < 0` and the flag is set and `unkC6 & 4` is clear; otherwise
   `unkC0` is a dwell timer decremented by dt, and on expiry — or when it
   was 0 with the flag set — it becomes `-1` (armed).
6. Direction 1 (up): `unk54 += dt * unkB8`; index advances when
   `unk54 >= thresholds[old]`, reversing at the top (`unkCB = 2`, index
   `old - 1`). Direction 2 (down): `unk54 -= dt * unkBC`, index decrements
   when `thresholds[old] >= unk54`, reversing to `unkCB = 1` below stop 0.
7. On an index change: `unk54` snaps to `thresholds[old]`, the timer becomes
   `1` for trigger mode or the per-stop dwell `unkAC[old]` otherwise,
   `unkC9` records the previous index, and a pending stop-sound
   (`unk21`/`unkCC` with flag bit `0x400000`) is stopped via
   `gStopSoundChannelByIndex(unk08, unk22)` (func_800457B8) with the flag
   bit cleared; `unkCD != 0` fires
   `gPlayPositionalSoundEffect(unkCD, unk50, unk58, -1)` (func_80047284).
8. The tail always runs (unless gates 2/3 fired): `gGteLoadMatrix(unk2C)`
   and one frame-vertex transform `gGteTransformVerticesWide(..., 1)`.

### Command interpreter (`gRunEntityCommands`, `src/game/80041728.c`)

Four `Command{op u8, arg u8}` slots at `+0x0C`; `op == 0` terminates the
walk. Opcodes:

| op | arg | behaviour |
|---|---|---|
| `0x53` | 0 / nonzero | set / clear bit 1 of `+0xC6` (freeze the lift) |
| `0x52` | 0 | `+0xC0 = -1` (arm the timer) |
| `0x52` | 1..10 | if `+0xC0 <= 0`: done; if `+0xC9` (current) equals `arg-1`: done; else index = `arg-1`, direction = `2` when the target is below the current stop else `1`, timer = `-1` |
| `0x52` | >= 11 | index = `arg-11`, timer = `-1`, `+0xC4` height = slot pointer table at `+0xA0[arg-11]`, immediate frame refresh via func_80010750 + func_80010234, loop exits |
| other | — | skipped, stays queued (`i++`); there is no default delegation despite the unit's older summary line |

Handled (`0x52`/`0x53`) commands are removed with
`gRemoveEntityCommand(self, i)` (func_8002A5B4) and the walk continues at
the same slot index.

### Shared field: the s16 at +0xC4

The s16 at `+0xC4` is read/written under three names across the family:
`Obj41B6C.count` (the msg-11 hit counter, capped at 20),
`Self.zero` of `gInitElevator` (cleared at init) and `Obj41880.unkC4` (the
hold countdown of `gStepElevator`). It is one shared block field, so a
msg-11 hit raises the same counter the tick later uses as a hold gate.
Verified: the three struct views in
`src/game/80041B6C.c:4-7`, `src/game/80041488.c:7`, `src/game/80041880.c:28-60`.

## The mounted-turret carrier chain (entity type 20)

### Spawn (`gSetupMountedTurretCarrier`, `src/game/80039504.c`)

Straight-line sequence: `gHandleType5Entity(2, self, other, arg3)` (the
class-5 init, func_800382B4), then
`flags = ((data->unk4C_word & 6) == 2) << 1` from `other->unk08` and
`self->unk158 = gSpawnMountedPartChild(self, data->unk48 << 8,
data->unk4A, flags, data->unk4C, data->unk4D, other->unk04->unk20)`
(func_80039418). Afterwards `self->unkA0 = 0`, the child's `unk24` gets flag
`0x10` ORed in, and `gQueueOrApplyMapOccupancy(self->unk158)`
(func_8003FA34) registers the child's map occupancy.

### Child spawn request (`gSpawnMountedPartChild`, `src/game/80039418.c`)

Builds a `Request{handler = func_80039298, args}` whose args carry kind
`0x15`, the parent's tag/parameter bytes, `flags & 0x7FFFDFFF` (bit `0x2000`
cleared), `unk1F = parent unk19 & 0xF`, and a position produced by
`gTransformPartVertex(object->coord, arg3, &args.position)` (func_80017668),
then queues it with `gSpawnQueuedEntity` (func_8004D444). The child's
handler is therefore `gHandleMountedPartMsg` itself.

### Carrier dispatch (`gHandleMountedTurretCarrierMessages`, `src/game/800395A8.c`)

| message | behaviour | return |
|---|---|---|
| 1 | block size | `0x15C` |
| 2 | `gSetupMountedTurretCarrier(msg, self, data, extra)` | `0` |
| 3 | skip on `unk24 & 0x20`; else base handler on self and, if present, on the child | `0` |
| 6 | class-5 on self; child: `unk1C = self->unk1C` and `gUpdateMountedTurret(child, data)` (func_80038A00) | `0` |
| 9, 11, 14 | class-5 on self; child: bit `0x02000000` of `unk24` propagated (set or cleared), health `unk1C` copied, and `gFlagQuartileChange(child, previous)` (func_8003BA50) fired when the child is alive (`unk20 != 0`) and `!(unk24 & 0x40000)`, with the old child health as the previous value | `0` |
| 5 | child `unk1C = -1`, falls through into the shared tail | undefined (no return in the source) |
| 4, 7, 8, 10, 12, 13, default | `gHandleType5Entity(msg, self, data, extra)` only | func_800382B4 result |

The case-5 fall-through tail has no `return` statement in the decompiled
source; the return value of messages 5/4/7/8/10/12/13 on the shared tail is
undefined retail behaviour and deliberately unasserted by the tests.

### Child container dispatch (`gHandleMountedPartMsg`, `src/game/80039298.c`)

Container object: `u16 flags` at `+0xE8`, child pointer at `+0xEC`;
child: `unk08` at `+0x08`, type byte at `+0x16`, countdown `unk1C` s16 at
`+0x1C`, slot count s8 at `+0x73`, object-pointer array at `+0x158`.

| message | behaviour | return |
|---|---|---|
| 1 | block size | `0xF0` |
| 2 | `gInitActorFromSpawn(obj, data, arg3)` (func_80038754), falls through | `0` |
| 3, 6 | nothing | `0` |
| 10 | identity query `obj->child != data` | `0` / `1` |
| 9 | `0` unless `flags & 1`, then base handler | base result / `0` |
| 5 | child type `0x1C`: walk `count` slots for `obj`, clear the match (`objects[0] = 0`); other types clear slot 0 directly. Then `child->unk1C = -1` unless `flags & 1`, and the base handler runs | base result |
| 11, 14 | `flags & 1` → base handler; else child type `0x14` → real `gHandleMountedTurretCarrierMessages` (return 0), type `0x1C` → `gHandleType28Entity` (func_800398EC, return 0), other types → `0` | `0` / base result |
| default | base handler | base result |

### Turret update (`gUpdateMountedTurret`, `src/game/80038A00.c`)

Runs on carrier message 6 (and on message-6 of the type-28 sibling
`func_800398EC`). Object view: model `+0x28` (anim `+0x10`, frame base
`+0x34`), basis `+0x2C`, position `+0x50`, output vertex `+0x5C`, frame
s16 `+0x70`, old positions `+0x78`, script words `+0xA0`, target flags
`+0xA6`, target `+0xB8`, anim track `+0xC8`/`+0xD4`, yaw `+0xDC`, pitch
`+0xDE`, speeds `+0xE0`, heading `+0xE4`, part `+0xE6`, mode `+0xE7`,
flags `+0xE8`, info pointer `+0xEC`. The update walks a model animation
lump (opcodes 1/2/3 via `func_80044B34 >> 1`), steers yaw/pitch toward a
target or the ground with dt-scaled clamped steps, and drives a 0/1/2 mode
machine through `func_80060238` / `func_800448D8` / `func_8005F6E8`.
Full behaviour is documented in the unit's own header; the host harness
covers it only through its `gUpdateMountedTurret` stand-in (see Test
coverage), so its internals stay structure-level here.

## Same-idiom neighbours (context, not part of the chains)

`gHandlePlacedPropMsg` (`src/game/80041C6C.c`, type 12, block `0xA4`) and
`gInitPortraitScreenEntity` (`src/game/80041E0C.c`, called by the type-34
handler `func_80042330`) share the lift chain's ground-snap/basis/frame
idiom (`gGetGroundHeight`, `gBuildRotationMatrix`, `func_80010750` +
`func_80010234`) but follow their own message maps; both are documented in
their unit headers.

## Globals

| symbol | role |
|---|---|
| `D_8009FA58` | two-slot player table `gPlayerEntities` walked by `gStepElevator` for the riding flag |
| `D_8009F800` | unit-spawner registry appended by `func_80060E74` |
| `D_80010E6C` | retail jump-table base of `func_8002AE54`'s type dispatch |

## Confirmed / Likely / Unproven

**Confirmed** (read from source and, where marked, exercised by the host
harness under qemu-mipsel):

- both handler bindings: type 10 → `0x80041B6C` (`0x8002AED0`), type 20 →
  `0x800395A8` (`0x8002AF18`), verified against the retail bytes of
  `SLUS_007.39` (file offset `addr − 0x80010000 + 0x800`).
- the full message maps of both dispatchers and the command-opcode table
  (harness cases A, C17–C22, E, F).
- the init arithmetic (`<<4` stops, `(v<<8)/218` speeds, `(v<<4)/300`
  derived values, end/last/mode/current derivation, ground snap applied to
  all stops, callee order) — harness case B.
- the gate order of the tick (commands → frozen → hold countdown → player
  flag → trigger/dwell arming → motion → stop-change events → matrix
  refresh), the ping-pong reversal at both ends, the trigger-mode dwell
  reload of constant 1, and both sound paths — harness cases C1–C16.
- the carrier chain: spawn descriptor reads, computed flags, child spawn
  request fields (kind 0x15, tag, `flags & 0x7FFFDFFF`, part index,
  position from the vertex transform), child registration, health/flag
  propagation with the quartile-change condition — harness cases D, E, F.
- the s16 at `+0xC4` is one shared field across all three lift-family
  struct views.
- **PA-Missionsplatzierung von Typ 10 und Typ 20 (Confirmed, alle 5 PA-`.MIS`-Dateien dekodiert über `Cact` `func_8004D6D4` `src/game/8004D6D4.c:103-165` und `Csac` `func_80060E74` `src/game/80060E74.c:142-157`):**
  - **Typ 10 (Lift, `gHandleElevatorEntity`):** **8 `Cact`-Instanzen** in 3 von 5 PA-Maps, **0 `Csac`-Spawner**:
    - `CONFT.MIS`: **4× `Cact`** (`sid=15` `SHDR@0x189068` / `TCAt+0xC@0x1890c4`, `sid=24` `0x189118`/`0x189174`, `sid=37` `0x1891c8`/`0x189224`, `sid=39` `0x189278`/`0x1892d4`), `0× Csac`.
    - `JOKE.MIS`: **2× `Cact`** (`sid=262` `SHDR@0x1b27a4` / `TCAt+0xC@0x1b2800`, `sid=249` `0x1b6a94`/`0x1b6af0`), `0× Csac`.
    - `SLIM.MIS`: **2× `Cact`** (`sid=262` `SHDR@0x1a2aa4` / `TCAt+0xC@0x1a2b00`, `sid=249` `0x1a7154`/`0x1a71b0`), `0× Csac`.
    - `HK.MIS` und `OVMP.MIS`: jeweils **0× `Cact`**, **0× `Csac`**.
  - **Typ 20 (Mounted-Turret-Carrier, `gHandleMountedTurretCarrierMessages`):** **0 Instanzen** in allen 5 PA-Missionsdateien (`CONFT.MIS`, `HK.MIS`, `JOKE.MIS`, `OVMP.MIS`, `SLIM.MIS`) — weder als `Cact` noch als `Csac`.
  - **Geschwistertyp 28 (`gHandleType28Entity`, `func_800398EC`):** **2× `Csac` pro PA-Map** (10 gesamt über alle 5 Maps, z. B. `CONFT.MIS` `Csac sid=7` und `sid=57`, ausgelöst durch die 50-Punkte-`RPNS`-Kaufskripte), **0× `Cact`**.

**Likely**:

- 218 and 300 in `gInitElevator` are unit conversions (≈1/256s and a
  1/300 scale), so `speeds` are per-second values in world units.
- the stop-sound pair (`func_800457B8`/`func_80047284`) starts/stops the
  lift motor at segment changes; `unk21`/`unkCC` stores a pending
  stop-sound request between ticks.
- flag bit `0x02000000` propagation to the child mirrors a "damaged"
  state, and `func_8003BA50` reports health-quartile changes for scoring
  or AI; the un-named registers pin the child's temporal state.

**Unproven**:

- whether the msg-11 hit counter (raising the tick's hold counter) pauses
  the lift in gameplay: the field identity is verified, the gameplay
  interpretation is not (which caller sends msg 11 with what cadence is
  unknown).
- why `gSpawnMountedPartChild` strips child flag bit `0x2000`.
- units of `info->h10C`/`info->limit` and the exact steering axes in
  `gUpdateMountedTurret`; the anim-lump opcode semantics beyond 1/2/3.
- `gHandleMountedPartMsg`'s unit `@param` note says the child pointer lives
  at `0xEA`; the struct layout of `src/game/80039298.c` places it at
  `+0xEC` (after `u16 flags` at `+0xE8` and two pad bytes), and the block
  size answer `0xF0` matches that layout.

## Retail data anchors (file:line)

- `src/game/8002AE54.c:70` — `case 10: return (s32)gHandleElevatorEntity;`
- `src/game/8002AE54.c:82` — `case 20: return (s32)gHandleMountedTurretCarrierMessages;`
- `src/game/80041B6C.c:37-59` — type-10 message switch with the capped counter at `case 11`
- `src/game/80041488.c:74-122` — script-record init and rescaling
- `src/game/80041880.c:155-258` — gate order, motion, stop-change events, matrix tail
- `src/game/80041728.c:69-145` — four-slot command interpreter
- `src/game/80039504.c:68-80` — carrier setup and child spawn
- `src/game/800395A8.c:52-115` — carrier message dispatch and child sync
- `src/game/80039298.c:64-137` — container message dispatch
- `src/game/80039418.c:91-121` — kind-0x15 spawn request assembly
- `src/game/80024EC8.c` — `"Csac"` → `func_80060E74` asset dispatch (map-placed entity source)
- `src/game/80060E74.c:143` — type byte `*(u8 *)(data->unk28 + 4)` → `gGetEntityHandlerForType`

## Test coverage

`tests/host/mover_objects_test.sh` builds `tests/host/mover_objects_harness.c`
plus the UNCHANGED retail units `80041B6C`, `80041488`, `80041880`,
`80041728`, `80039504`, `800395A8`, `80039298`, `80039418` with the pinned
gcc-2.7.2-cdk + maspsx + fix_align pipeline (identical to
`spawner_path_test.sh`) and runs them under qemu-mipsel with the
`retail − 0x70000000` memory window at `0x10010000`. GTE entry points
(`func_80010750`, `func_80010234`, `func_800188D0`) and all other callees
are recording stand-ins bound by asm alias; no cop2/GTE asm unit is linked.

Executed (62 cases, 252 checks, real output):

- `gHandleElevatorEntityMessages`: all arms 1/2/5/11/default/6 via the real
  init and step units — the 20-cap of the hit counter (19→20, 20→20, 5→6,
  −1→0), callee order, and argument forwarding.
- `gInitElevator`: full record copy/rescale, ground snap, end/last/mode
  derivations (both `end < count` and `end >= count`), current selection,
  volatile previous/old round-trip, callee order and args, frame refresh.
- `gStepElevator` + real `gRunEntityCommands`: frozen bit, hold countdown,
  both directions, both reversals, stop-change snaps, dwell reload
  (per-stop and trigger-mode constant 1), timer-0 player arming, busy
  player, trigger-mode gating, both sound paths, matrix tail, opcodes
  0x52/0x53 in every documented argument range, unknown-opcode persistence.
- `gSetupMountedTurretCarrier` + real `gSpawnMountedPartChild`: callee
  order, computed flags, every forwarded spawn-request field, child
  registration, occupancy queue.
- `gHandleMountedTurretCarrierMessages`: all arms, child health/flag
  propagation in both directions, quartile-change gating (live/dead child,
  0x40000 bit), null-child handling.
- `gHandleMountedPartMsg`: all arms, the 0x1C slot walk (match/no match),
  direct slot-0 clear, countdown gate, the 0x14 → real carrier and 0x1C →
  type-28 forwards, identity query, flag gates.

Mutation probe: raising the msg-11 cap (20 → 21) in a temporary copy of
`80041B6C.c` (repo untouched) turns the suite red exactly at the cap check
(`dispatch: msg 11 caps the counter at 20 got 21 want 20`).

**Not covered:** `func_80038A00` internals (stand-in; its 2200-byte
animation/aim machine is characterised only in the unit header),
`func_80041C6C`/`func_80041E0C` (context-only neighbours, no host tests
here), the case-5 fall-through return value (undefined retail), and any
level-data-driven behaviour (no placement data exists in the repo).

## Open questions

1. *(Geklärt — siehe Confirmed oben)* Typ 10 wird 8× als `Cact` in PA platziert (`CONFT.MIS`: 4, `JOKE.MIS`: 2, `SLIM.MIS`: 2; `HK.MIS`/`OVMP.MIS`: 0), Typ 20 kommt in keiner der 5 PA-`.MIS`-Dateien vor (0× `Cact`, 0× `Csac`), während Geschwistertyp 28 in jeder PA-Map 2× als `Csac` vorkommt.
2. Who sends message 11 to a type-10 entity, and does the shared `+0xC4`
   counter measurably pause the lift in gameplay?
3. Why is child flag bit `0x2000` stripped in the spawn request?
4. The exact steering axes/units in `gUpdateMountedTurret` and the full
   anim-lump opcode set.
5. Should `func_80041C6C`/`func_80041E0C` get their own harness (same
   ground-snap idiom), or stay structure-level here?
