# Map-Placed Objects, Base Ownership and Unit Spawners (Precinct Assault)

## Overview

Precinct Assault places a level's structures as *map objects*: entity records that the level loader instantiates and that carry a slot index into the mission's spawn-descriptor table `D_800A0C70`. Two behaviours hang off that table, and they are the entire mechanic this document covers:

1. **Occupancy registration** — every freshly spawned map object asks to be written into the occupancy grid, either immediately or (while the level is still being built) by stacking an 8-byte request that `func_8003FAD4` replays after the load finishes.
2. **Base ownership** — the subset of these objects with entity kind `0x24` are *objectives*. A nearby attacker of a valid class flips the objective's team latch, writes the new owning team onto the objective and its child part, and — through the message-5 handler — books the claim in the match block and the player's tracker.
3. **Unit production** — structures that produce units do so from a *spawner record* that the level loader registers at load time (`func_80060E74` → `D_8009F800`) and a per-frame ticker (`func_800616A0`) drains. There is no player-facing build step anywhere in that chain.

Everything below was read out of `src/game/*.c`, `config/us/matching_units.json`, and the retail image `baseroms/us/SLUS_007.39`. Where a claim rests on inference rather than on code, it is marked **Likely** or **Unproven**.

### What is *not* in the retail code

**Finding (confirmed, not a gap):** there is no purchase UI, no tank shop, no player-facing build menu, and no cost/price accounting for units. Evidence:

* A byte scan of `baseroms/us/SLUS_007.39` (589 824 B, PS-X EXE, text at `0x80010000`, size `0x8F800`) finds the six objective/score labels `Points Scored` (`0x80010C24`), `Current Points` (`0x80010C34`), `Outposts Claimed` (`0x80010C44`), `Neutral Turrets` (`0x80010C58`), `SuperFortresses` (`0x80010C68`), `Dreadnoughts` (`0x80010C78`) and the unit-class labels `Tanks` / `Jets` / `Walkers` (`0x8009F298`, `0x8009F290`, `0x8009F288`) — all of them entries of one statistics descriptor at `0x8009AB60..0x8009ABB4`. They are scoreboard rows, not menu items.
* The same scan finds **zero** occurrences of `buy`, `Buy`, `purchase`, `cost`, `Cost`, `price`, `Price`, `build`, `Build` anywhere in the image.
* The only `cost`-named code in `src/game/` is the controller-pak actuator budget (`src/game/8008C138.c:88`, `gPadActuatorCostSum < 61`) and comments about distance/segment cost. Nothing spends or checks money for units.
* `gRegisterUnitSpawner` (`src/game/80060E74.c:112`) takes an asset descriptor that already exists, resolves its three sub-sections, and publishes it. It has no price field, no owner check, and no caller other than the asset loader (`Confirmed: func_80024EC8`).

So the mechanic the code implements is: **capture a map objective → own it → the objective's spawner record keeps producing the units its order word names.** Ownership changes *who* the produced units belong to via the objective's team byte; it never gates a purchase.

## Hop-by-hop path

1. **Level spawn state init (`0x8003F79C`, `func_8003F79C` / `gInitMissionSpawnState`)**: called from the module loader `func_80024A24` (`src/game/80024A24.c:223`, declared at `:69`). Clears all 256 slots of `D_800A0C70` (`src/game/8003F79C.c:50`), both 256-word banks of `D_800A7040`, seven words of `D_800A1070`, `D_8009F310`, `D_8009F314`, then allocates the deferral pool: `gDeferredRegPool = func_800194DC(0x100, 8, 1)` (`src/game/8003F79C.c:63`) — 256 nodes of 8 bytes, tag 1.
2. **Map object spawn (`0x8002A5F4`, `func_8002A5F4` / `gInitEntityBase`)**: builds the entity from its source record (position `:159-160`, definition id `:118`, the four `slots[].active` flags cleared `:102-104`, the slot index copied from `src->unk20` `:127`, `+0x90 = 0x7F00` `:166`) and, as its last action, calls `gQueueOrApplyMapOccupancy(self)` (`src/game/8002A5F4.c:167`).
3. **Occupancy bookkeeping (`0x8003FA34`, `func_8003FA34` / `gQueueOrApplyMapOccupancy`)**: two ways through, decided by three words:
   * immediate — `gDeferredRegPool == 0` **or** `gScreenState[0] == 4` **or** `gScreenState[0] == 0xD` (`src/game/8003FA34.c:56`), in which case `gSpawnEntityPrimaryType(object)` runs at once;
   * deferred — otherwise, and only when `gGetSpawnDescColumn(object) != 0` (`:60`), take an 8-byte node from the pool (`:61`), store the object in it, and **prepend** it to `D_8009F708` (`:64-69`). A null node (pool exhausted) silently drops the request.
4. **The write itself (`0x8003BC0C`, `func_8003BC0C` / `gSpawnEntityPrimaryType`)**: returns immediately when the object's slot byte (`+0x20`) is 0; otherwise resolves the column with `gGetSpawnDescColumn` and calls `gSpawnTypeAtOwner(object, column, 0, gGetEntitySpawnBank(object), 0)` (`src/game/8003BC0C.c:54-58`). `func_800402DC` then looks up `D_800A7040[row][column]` and does the actual object construction.
5. **Slot column lookup (`0x8003B838`, `func_8003B838` / `gGetSpawnDescColumn`)**: three guards — slot byte 0, flag `0x40000` (self-positioned: the object supplies its own position, so it must not claim a map cell), and a null `D_800A0C70[slot]` — then returns `entry[0]` (`src/game/8003B838.c:56-66`).
6. **Replay on load finish (`0x8003FAD4`, `func_8003FAD4` / `gFinishLevelLoad`)**: called from `func_800287F4` (`src/game/800287F4.c:82`, declared at `:16`). Marks both spawn-record chains (`gMarkSpawnRecordChains(0)`/`(1)`, `:81-82`), walks `D_8009F708` calling `gSpawnEntityPrimaryType(node->object)` (`:84-87`), releases the pool (`:89`), then zeroes both `gDeferredRegPool` and `gDeferredRegList` (`:97-98`) and continues with the grey-texpage/HUD setup.
7. **Objective capture (`0x8002D664` msg 10 → `0x8002D1A0`)**: the objective is an entity of kind `0x24`. Message 10 reaches `gTryCaptureObjective(obj, arg2)` only for that kind (`src/game/8002D664.c:141-143`), and the handler returns "neither child part matched" (`:145-148`).
8. **Proximity + ownership (`0x8002D1A0`, `func_8002D1A0` / `gTryCaptureObjective`)**: see the sequence below.
9. **Scoring on message 5 (`0x8002D664` msg 5)**: for kind `0x24` with the latch set and flag `0x4000000` clear (`:183`), the handler derives the *other* team id (`:184`) and takes one of two branches. Single player with a matching team (`:185-207`): `gGameSession->unk68++`, `unk80++`, `gPlayerEntities[0]->unkAC->unk10++`, the player's `unk134->unk18` cleared, then the capturing unit is looked up by `obj->unk8C & 0x3FFFFFFF` (`gFindUnitById`) and banner `unkE0 = 7` is raised when it belongs to the single player. Split screen (`:208-221`): `+0x64`/`+0x7C` bumped at the slot's `unkC1` index and `players[i ^ 1]->unkAC->unk10++`. Both branches end with `gEntityBaseHandler(message, obj, arg2, arg3)`.
10. **Unit-spawner record registration (`0x80060E74`, `func_80060E74` / `gRegisterUnitSpawner`)**: appends the descriptor to `D_8009F800`, resolves the `'CASt'` / `'LSRa'` / `'TCAt'` views, converts the respawn delay, seeds the first delay, and stores the per-type handler.
11. **Unit production per frame (`0x800616A0` → `0x8006125C`)**: `gUpdateRegisteredSpawners` walks `D_8009F800->items[0..count-1]`, drains queued spawn bits via `gTrySpawnRegisteredUnit(entity, 1)` / `(entity, 0)` (`src/game/800616A0.c:63,66`), clears those bits, and fires `gTrySpawnRegisteredUnit(entity, 1)` when the interval timer underflows (`:74`). This half is already host-tested — see *Test coverage*.

## Sequence: proximity capture (`func_8002D1A0`)

Reading `src/game/8002D1A0.c:126-240` in order:

1. `if (self->unkFE != 0) return;` (`:137`) — the latch is one-shot: an already-claimed objective is inert.
2. Team gate: the attacker's team byte `other->unk18` must equal `self->unkF6` or `self->unkF7` (`:138`). The two bytes are the objective's "own" and "contested" team ids.
3. Class gate (`:140-152`):
   * `other->unk16 == 1` (player) or `0x12` (unit carried by something else) — for `0x12` the carrier `other->unkB4` is dereferenced and a null carrier aborts (`:141-144`); then a live player slot must exist whose `unk14` matches the attacker's and whose `unkAC->unk40 == 0` (`:145-148`).
   * `other->unk16 == 5` (vehicle) or `0x25` (flat-test class) pass straight through; every other class aborts (`:150-152`).
4. Range test (`:154-170`): a manhattan/octagonal approximation of `dist(self, other)` in the XZ plane, computed with the same shift-add sequence twice (once for `+0x34/+0x3C`, once — for the 3D classes — for `+0x38`). `limit = (self->unkFC << 4) + other->coord.radius + 0x4000` (`:169`); `dist - limit >= 0` aborts. Class `0x25` compares the 2D distance instead and skips the second axis entirely (`:172-175`).
5. Ownership transfer (`:197-212`), all in one pass:
   * `mode = (id == self->unkF6) ? 1 : 2`, where `id = other->unk18`;
   * `self->unkFE = mode` — this is the latch *and* the mode;
   * `self->unk18 = id` and `self->unk104->unk18 = id` — objective and its child part change hands;
   * `self->unkA6 = 4`, `self->unkDE = 4` — state bytes;
   * `self->unkA8 = (mode == 1) ? self->unkF7 : self->unkF6` and `self->unkDC = self->unkA8` — the *other* team is recorded as the new challenger;
   * `self->unk23 = (mode == 1) ? self->unkF8 : self->unkF9` — a kind byte picked by mode;
   * `self->unk92 += self->unkFA * mode` and `self->unk93 += self->unkFB * mode`, both copied to the child — **the score increment is scaled by the mode**, so a steal (mode 2) is worth twice a claim (mode 1);
   * `self->unk24 &= ~1` — flag bit 0 cleared.
6. Counter propagation (`:213-237`): with `gPlayerCount == 1` and the single-player entity's team matching, bump `gGameSession->unk64[1]`, `unk7C[1]`, the player's `unk134->unk3A`, clear `unk134->unk18`, and set `unkE0 = 6` when the capturing unit's low 14 bits match the player's id; otherwise (split screen) bump `unk64[slot]`, `unk7C[slot]` and the slot's `unkAC->unk12` for each live player whose team matches.
7. Script hook (`:238-239`): when `self->unk9C` is a non-null pointer whose first byte is non-zero **and** flag `0x100000` is set in `self->unk24`, call `gInterpretEntityScript(self->unk9C, 0, other->unk14 & 0x7FFF)`. The capture itself already happened; the script is a notification, not a precondition.

## The spawner record (`func_80060E74`)

`src/game/80060E74.c:112-159`:

| step | code | effect |
|---|---|---|
| publish | `*(gRegistryTable->entries + gRegistryTable->count++) = arg0;` (`:122`) | the descriptor becomes a spawner, appended at the live count |
| main record | `data = func_80019CA0(arg0->unk00, arg0->unk20, 0x74534143 'CASt', arg0->unk1C)` (`:124`) | the `'CASt'` header for this section id |
| stale field | `arg0->unk08 = 0` (`:126`) | |
| respawn delay | `data->unk0C = data->unk0C * 5` (`:129`) | raw units are scaled by 5 |
| first delay | negative → mirrored and used directly (`:130-132`); otherwise `(u32)func_80019668() % data->unk0C` (`:135`) | a *positive* delay consumes one PRNG draw, a negative one does not |
| spawn counter | `data->unk16 = 0` (`:128`) | |
| data window | `data->unk18 = arg0->unk38`, `data->unk1C = data->unk18 + arg0->unk30` (`:138-139`) | the payload is **inline** at the descriptor's own `+0x38` |
| entry list | `first = func_80019CA0(..., 'LSRa', ...)`, `list->count = (first->unk04 - 0xC) >> 3` (`:141,148`) | count derived from the section's byte length |
| trailing table | `data->unk28 = (u8 *)func_80019C38(..., 'TCAt', ...) - 4` (`:142,144`) | header-relative, hence the `-4` |
| entry resolution | for each entry with a non-zero name: `entries[i].unk04 = func_80019BF0(group, name)` (`:150-155`) | tag pair → direct pointer |
| handler | `data->unk20 = gGetEntityHandlerForType(*(u8 *)(data->unk28 + 4))` (`:157`) | the type byte is the first `'TCAt'` payload byte |
| back link | `data->unk2C = arg0` (`:158`) | |

The record therefore carries: a respawn interval, a current countdown, a spawn counter, a pointer range over its own data, a resolved entry table, a trailing type table and an update handler. Nothing in it is a price.

## Function table

Sizes are from `config/us/matching_units.json` (`size`), addresses from the same file and confirmed against the `glabel` in `asm/us/main.s`.

| address | retail symbol | current C name | role | size |
|---|---|---|---|---|
| `0x8003F79C` | `func_8003F79C` | `gInitMissionSpawnState` | Clears `D_800A0C70`/`D_800A7040`/`D_800A1070`, zeroes `D_8009F310`/`D_8009F314`, allocates the 256×8 deferral pool | `0xB8` |
| `0x8002A5F4` | `func_8002A5F4` | `gInitEntityBase` | Spawns a map object from its source record; ends with the occupancy request | `0x21C` |
| `0x8003FA34` | `func_8003FA34` | `gQueueOrApplyMapOccupancy` | Immediate vs. deferred occupancy registration | `0xA0` |
| `0x8003BC0C` | `func_8003BC0C` | `gSpawnEntityPrimaryType` | The only occupancy writer: column → `func_800402DC` | `0x70` |
| `0x8003B838` | `func_8003B838` | `gGetSpawnDescColumn` | Slot descriptor → primary column byte, three guards | `0x60` |
| `0x8003BF50` | `func_8003BF50` | `gGetEntitySpawnBank` | Kind byte → bank word (`1`, or `0x12` via a carrier link) | `0x80` |
| `0x8003B65C` | `func_8003B65C` | `gQuarterIndex` | Value/range → quarter 0..3 | `0x48` |
| `0x8003BA50` | `func_8003BA50` | `gFlagQuartileChange` | Sets `0x4000` when the quarter changed and a descriptor exists | `0x94` |
| `0x8003BAE4` | `func_8003BAE4` | `gRefreshEntitySlotSpawns` | Consumes `0x4000`: places up to five columns via `func_800402DC` | `0x128` |
| `0x8003B898` | `func_8003B898` | `gSpawnEntitySlotTypes` | Multi-column occupancy for footprinted objects | `0x1B8` |
| `0x8003BC7C` | `func_8003BC7C` | `gSetSlotSpawnDescriptor` | Rebuilds one `0x1C` slot snapshot from a `'tACT'` record | `0x124` |
| `0x8003BDA0` | `func_8003BDA0` | `gLoadActorDef5B` | `'tACT'` lookup → `gSetSlotSpawnDescriptor`, frees the descriptor | `0x4C` |
| `0x8003FAD4` | `func_8003FAD4` | `gFinishLevelLoad` | Replays the deferral list, releases the pool | `0xBC` |
| `0x8004CE04` | `func_8004CE04` | `gProcessDeferredEntityUpdates` | Per-frame `0x4000` consumer → `gRefreshEntitySlotSpawns` | `0xC8` |
| `0x800402DC` | `func_800402DC` | `gSpawnTypeAtOwner` | `D_800A7040[row][column]` → construct the object | `0xF4` |
| `0x8002D664` | `func_8002D664` | `gHandleLinkedPropEntity` | Message dispatch; msg 10 → capture, msg 5 → claim scoring | `0x440` |
| `0x8002D1A0` | `func_8002D1A0` | `gTryCaptureObjective` | Proximity + team + class → ownership transfer + score | `0x4C4` |
| `0x80069D68` | `func_80069D68` | `gInterpretEntityScript` | Script VM entered after a successful capture | `0x7BC` |
| `0x80060E74` | `func_80060E74` | `gRegisterUnitSpawner` | Appends and resolves a spawner record | `0x1AC` |
| `0x800616A0` | `func_800616A0` | `gUpdateRegisteredSpawners` | Per-frame drain of every spawner (already tested elsewhere) | `0x138` |
| `0x8006125C` | `func_8006125C` | `gTrySpawnRegisteredUnit` | One spawner's burst/timer step (already tested elsewhere) | `0x260` |
| `0x800195A4` | `func_800195A4` | `gTakeFixedPoolSlot` | Pops a node from a fixed pool | `0x2C` |
| `0x800194DC` | `func_800194DC` | — | Allocates the 256×8 pool | `0xA8` |
| `0x80019CA0` | `func_80019CA0` | `gFindSectionHeader` | Tag/id → section header inside a blob | `0x60` |
| `0x80019C38` | `func_80019C38` | `gFindSectionPayload` | Tag/id → section payload | `0x68` |
| `0x80019BF0` | `func_80019BF0` | `gFindRegisteredResource` | (group, name) → registry node | `0x48` |
| `0x80019A74` | `func_80019A74` | `gAppendResourceRegistryEntry` | Appends to the resource registry | `0x34` |

## Global table

| `D_` symbol | current C alias | meaning | evidence |
|---|---|---|---|
| `D_800A0C70` | `gSlotSpawnDescs` | 256-entry pointer table of `0x1C`-byte slot snapshots; index 0 = unassigned | cleared in `src/game/8003F79C.c:50`, rebuilt in `src/game/8003BC7C.c:45-60`, read in `src/game/8003B838.c:62` |
| `D_8009F70C` | `gDeferredRegPool` | deferral pool descriptor (`count` `+0x0C`, `head` `+0x10`), 256×8 bytes | allocated `src/game/8003F79C.c:63`, consumed `src/game/8003FA34.c:61`, released/zeroed `src/game/8003FAD4.c:89,97` |
| `D_8009F708` | `gDeferredRegList` | head of the 8-byte `{next, object}` stack | pushed `src/game/8003FA34.c:64-69`, walked `src/game/8003FAD4.c:84-87` |
| `D_8009FA00` | `gScreenState` | Screen-Zustand: `4` ist der interaktive Zustand des geladenen Moduls — Front-End-Menue **und** laufendes Spiel —, geschrieben ausschliesslich vom `'Cctr'`-Asset-Chunk (`unk1C == 1` → `4`, `unk1C == 0` → `2`); `0xD` ist der Nach-Countdown-Zustand (Ergebnisanzeige laeuft, HUD aus) | `src/game/80024EC8.c:175-187`, `src/game/80027D9C.c:76-91`, `src/game/8003FA34.c:56` |
| `D_800A7040` | `gSpawnRecordMap` | `[2][256]` per-bank spawn definition table read by `func_800402DC` | cleared `src/game/8003F79C.c:54`, read `src/game/800402DC.c:81` |
| `D_800A1070` | `gListHeads` | seven list heads cleared at level init | `src/game/8003F79C.c:67` |
| `D_8009F310` | `gSpawnTableRow` | active bank/row word | zeroed at `src/game/8003F79C.c:60` |
| `D_8009F314` | `gPendingSpawnList` | pending spawn list, zeroed at level init | `src/game/8003F79C.c:64` |
| `D_8009F800` | `gRegistryTable` | **pointer** to `{count, entries[]}` — the unit-spawner registry | written `src/game/80060E74.c:122`, read `src/game/800616A0.c:59` |
| `D_8009F5B8` | `gResourceListHead` | resource registry head | `src/game/80019A74.c:10` |
| `D_8009F5BC` | `gResourceListTail` | resource registry tail | `src/game/80019A74.c:11` |
| `D_8009FA0C` | `gPlayerCount` | 1 = single player, otherwise split screen | `src/game/8002D1A0.c:216` |
| `D_8009FA58` | `gPlayerEntities[2]` | the two player entity slots | `src/game/8002D1A0.c:145` |
| `D_8009F9EC` | `gSinglePlayerEntity` | single-player entity | `src/game/8002D1A0.c:216` |
| `D_8009FA94` | `gGameSession` | match block: `+0x64`/`+0x7C` per-slot claim and total counters, `+0xE0` banner id | `src/game/8002D1A0.c:217-222` |

## Retail data anchors

* **Scoreboard labels** — `baseroms/us/SLUS_007.39`, text loaded at `0x80010000` (PS-X EXE header: text address `0x80010000`, size `0x8F800`):

  | label | file offset | load address | pointer cell |
  |---|---|---|---|
  | `Points Scored` | `0x1424` | `0x80010C24` | `0x8009ABA0` |
  | `Current Points` | `0x1434` | `0x80010C34` | `0x8009AB9C` |
  | `Outposts Claimed` | `0x1444` | `0x80010C44` | `0x8009AB94` |
  | `Neutral Turrets` | `0x1458` | `0x80010C58` | `0x8009AB90` |
  | `SuperFortresses` | `0x1468` | `0x80010C68` | `0x8009AB8C` |
  | `Dreadnoughts` | `0x1478` | `0x80010C78` | `0x8009AB88` |
  | `Tanks` / `Jets` / `Walkers` | `0x8FA98` / `0x8FA90` / `0x8FA88` | `0x8009F298` / `0x8009F290` / `0x8009F288` | `0x8009AB80` / `0x8009AB84` / `0x8009AB98` |
  | `First Outpost` | `0x13FC` | `0x80010BFC` | `0x8009ABAC` |

  The pointer cells form one contiguous descriptor `0x8009AB60..0x8009ABB4`, i.e. a stat-screen row table (`Level 10`, `LOSER`, `Total Time`, `First Death`, `Time as Walker`, `Time as Hovercraft` complete the block). The `Outposts Claimed` row is exactly the `unk64`/`unk7C` counter that `func_8002D1A0` increments, which is why the claim really is an "outpost claim" and not a generic capture.
* **Absent strings** — the same image contains no `buy`/`purchase`/`cost`/`price`/`build` occurrence at all (byte scan over the whole 589 824-byte file). That is the evidence for the "no purchase UI" finding above.
* **Section tags** — `'CASt'` = `0x74534143`, `'LSRa'` = `0x6152534C`, `'TCAt'` = `0x74414354`, used as the `func_80019CA0`/`func_80019C38` tag argument in `src/game/80060E74.c:124,141,142` and `src/game/8003BDA0.c:47`.
* **Instruction-order anchors** (read-only parent checkout, `asm/us/main.s`): `glabel func_8003FA34` at line 55 923 (50 lines), `func_8003FAD4` at 55 973, `func_8003F79C` at 55 727, `func_8003BC0C` at 51 473, `func_8003B838` at 51 193, `func_8003BA50` at 51 346, `func_8003B65C` at 51 043, `func_8003BF50` at 51 727, `func_8002D1A0` at 34 832, `func_8002D664` at 35 169, `func_80060E74` at 94 408, `func_800616A0` at 95 013.
* **Global addresses via `$gp`** — `func_8003FA34` loads `D_8009F70C` from `0x4F8($gp)` and `D_8009F708` from `0x4F4($gp)`; the pool/list pair is adjacent, consistent with the `+0x0C`/`+0x10` field offsets the pool descriptor uses.
* **`matching_units.json` sizes** — the seven units annotated for this document are `0x4C4` (`8002D1A0`), `0x440` (`8002D664`), `0xA0` (`8003FA34`), `0x70` (`8003BC0C`), `0x60` (`8003B838`), `0x94` (`8003BA50`), `0x1AC` (`80060E74`).

## Test coverage

`tests/host/map_objects_test.sh` compiles the unchanged retail C of thirteen units with GCC 2.7.2 + maspsx and runs them under `qemu-mipsel`:

```
8003FA34 8003BC0C 8003B838 8003BF50 8003B65C 8003BA50 800195A4
8002D1A0 80060E74 80019CA0 80019C38 80019BF0 80019A74
```

`func_800402DC` is *not* linked — it is the boundary: a recording stub keeps its five arguments, so the suite observes what the occupancy chain decided without needing the whole object constructor. `func_80069D68` (script VM), `func_80019668` (PRNG) and `func_8002AE54` (handler factory) are stubs for the same reason.

Run it from the worktree root (`bash tests/host/map_objects_test.sh`, builds into the gitignored `tests/host/build-map-objects`). It prints `map-objects: ALL PASS (N cases, M checks)` and exits non-zero on the first failing check.

Result: **36 cases, 165 checks, all pass.**

| area | what the suite pins |
|---|---|
| `func_8003FA34` (cases 1–13) | pool absent → immediate; screen state 4 → immediate; state 0xD → immediate; state 5 → deferred; slot index 0 → dropped *with* the pool untouched; flag `0x40000` → dropped; null descriptor → dropped; descriptor with column 0 → dropped; empty pool → dropped; first deferral onto an empty list; second deferral **prepended** onto a non-empty list; third deferral against an exhausted pool leaves head and links intact; immediate branch still honours the column guard |
| `func_8003B838` | reads `entry[0]`; index 0 / flag `0x40000` / null descriptor / unused index all return 0; other flag bits do not mask |
| `func_8003B65C` | the exact thresholds (`<= range>>2` → 0, `< range>>1` → 1, `< 3*range>>2` → 2, else 3), including `range == 0` and a negative value |
| `func_8003BA50` | crossing sets `0x4000`; same quarter does not; the flag is OR-ed onto the existing word; a *downward* crossing flags too; negative counter / null descriptor / zero range / index 0 never flag |
| `func_8002D1A0` (13 cases: `capture: ...`) | latch one-shot; wrong team; unsupported class; player class with a live slot; inactive player slot; class `0x12` with null carrier and with a live carrier; mode 2 (steal) latches `2` and scores `3×2`/`2×2`; team/child/`unkA8`/`unkDC`/`unk23`/`unkA6`/`unkDE` propagation; flag bit 0 cleared *and* the `0x100000` gate preserved; default radius in/out; radius from `unkFC` and from the attacker's own radius; class `0x25` ignores height while class 5 does not; single-player counters + `unkE0 = 6`; split-screen slot-indexed counters with no banner; script gate (flag clear, null pointer, empty first byte) |
| `func_80060E74` (synthetic three-section blob) | registry append at the live count (earlier entries untouched); `'CASt'`/`'LSRa'`/`'TCAt'` walk; delay `×5`; positive delay consumes exactly one PRNG draw and uses `rand % delay`; negative delay mirrored with **no** draw; payload window = descriptor `+0x38`; entry count from the section length; entry pointer resolution and the unresolved-name case; trailing table at `payload - 4`; handler taken from the first `'TCAt'` payload byte; `+0x08` cleared; back-link stored |
| `func_800195A4`, `func_80019CA0`, `func_80019C38`, `func_80019BF0`, `func_80019A74` | pool pop/head-move/count and the empty-pool case; tag/id section lookup incl. id 0 short-circuit; registry append order, tail linking and key-pair lookup with a miss |

**Already covered elsewhere (cited, not duplicated):**

* The runtime half of unit production — `func_800616A0` / `func_8006125C` / `func_8006158C` / `func_800614BC` — is host-tested in `tests/host/spawner_path_test.sh` (34 cases).
* `func_8003BA50` is also linked in `tests/host/leaf_fifty_b_harness.c` (5 checks: negative `unk1C`, null descriptor, same quarter, `90 → 60` crossing, `unk1C == 0` boundary). This suite adds the exact threshold operators of `func_8003B65C`, the OR-onto-existing-word behaviour, the downward crossing, the zero-range case and the index-0 case.

**Branches NOT covered by this suite:**

* `func_8003FAD4` (the replay itself) — the suite drives the deferral path but does not run the replay; the `{next, object}` layout and the prepend order are asserted instead.
* `func_8003B898` / `func_8003BAE4` — the multi-column footprint writer and the `0x4000` consumer. They call `func_800402DC` and `func_8003B5AC`, i.e. they would need the whole grid, so they are documented but not executed here.
* `func_8002D664` — the message dispatcher is documented from source; the capture is tested directly at its callee. Message 5's scoring branch (the `unk68`/`unk80`/banner-7 half) is **not** executed.
* `func_8003BF50`'s carrier branches (`kind 7` via `+0xC0`, `kind 2`/`0xF` via `+0xC8`) — linked but only the `kind 1` path is exercised.
* `func_800402DC`'s cell-flag and deferred-request paths — stubbed at the boundary by design.
* `func_80069D68` (script VM) — stubbed; the suite only pins *when* it is called and with which id.

## Confirmed

* `func_8003FA34` has exactly three immediate conditions (`pool == 0`, screen state `4`, screen state `0xD`), and defers only when the column lookup also succeeds. `src/game/8003FA34.c:56-71`.
* The deferral list is a **stack**: new nodes become the head, so `func_8003FAD4` replays newest-first. `src/game/8003FA34.c:64-69` and `src/game/8003FAD4.c:84-87`.
* An exhausted pool silently drops the request; nothing is logged and the object keeps whatever occupancy it had. `src/game/8003FA34.c:61-62`.
* The pool is 256 nodes of 8 bytes, tag 1, allocated per level and zeroed at teardown. `src/game/8003F79C.c:63`, `src/game/8003FAD4.c:89,97`.
* `func_8003B838` refuses to give a column when the object is self-positioned (`flags & 0x40000`). `src/game/8003B838.c:59-61`.
* `func_8003BC0C` writes with `arg2 = 0` and `arg4 = 0` and takes the bank from `func_8003BF50`. `src/game/8003BC0C.c:57`.
* Capture requires: latch clear, team match against `unkF6`/`unkF7`, a valid attacker class (`1`, `0x12`, `5`, `0x25`), and distance below `(unkFC << 4) + radius + 0x4000`. `src/game/8002D1A0.c:137-170`.
* Capture mode is `1` when the attacker's team equals `unkF6`, else `2`, and the mode multiplies both score increments. `src/game/8002D1A0.c:197-211`.
* Ownership is written to the objective **and** to `unk104` (the child part). `src/game/8002D1A0.c:200-201`.
* The script VM is called only after the transfer, and only with flag `0x100000` set and a non-empty script. `src/game/8002D1A0.c:238-239`.
* Only kind `0x24` objectives can be captured. `src/game/8002D664.c:141-143`.
* Claim scoring (msg 5) is a separate step from the capture, gated on the latch and on flag `0x4000000` being clear. `src/game/8002D664.c:183`.
* `func_80060E74` scales the respawn delay by 5, seeds the first delay from the PRNG only on the non-negative branch, and resolves the entry table from the section length. `src/game/80060E74.c:129-155`.
* **The screen-state machine of `D_8009FA00` (`gScreenState`) — every writer enumerated.** Reads like `src/game/8003FA34.c:56` treat states 4 and 0xD as "the level's grid exists", and the frame loop `src/game/80028618.c:191` runs in 4, 8 and 0xD; all other states pause gameplay. Value-by-value:
  - `1` — loading a module: `gStartModuleByName` (`src/game/80024E3C.c:55`) writes 1 before the module files are fetched (the state exists until loading finishes).
  - `2` / `4` — the `'Cctr'` asset chunk sets the module's interactive state when the level's own control chunk arrives: `unk1C == 0 → 2`, `unk1C == 1 → 4` (`src/game/80024EC8.c:175-187`). In 4 the front-end menu loop (`while (gScreenState == 4)`, `src/game/80027EE4.c:54`) and the gameplay frame loop (`src/game/80028618.c:191`) both run their per-frame work; `4` is therefore *the whole interactive lifetime of a module* — menu and match — and the "level build done" signal for the occupancy chain (hence the immediate write in `func_8003FA34`).
  - `5` — generic "leave this module" exit, written by five UI actions: option/quit confirm (`src/game/800260B8.c:228`), attract-demo launch (`src/game/80078C88.c:92`), loadout/mission confirm (`src/game/8007E3EC.c:90`), demo playback end (`src/game/8002703C.c:102`) and the results screen's continue check (`src/game/80028164.c:194`); `gFinishScreenModule` then maps 5 → `unk10 = 2` (next module, `src/game/80025B7C.c:91-93`).
  - `6` — player retirement fade: `gTickPlayerTransitionFade` writes 6 when a player's `gTeamFlag` value reaches `0x384` (`src/game/80026F68.c:43`); finish maps it to `unk10 = 2`.
  - `0x0B` (11) — countdown expiry *without* session mode 1 (`src/game/80027D9C.c:90`) or the cheat `0x01ADCFED` else-branch (`src/game/80060A0C.c:120`); finish maps it (default arm) to `unk10 = 4`, i.e. back to a state-4 module.
  - `0xE` (14) — results-screen transition (`src/game/80028164.c:198`); needs the same dispatcher caveat as 8 (no writer of the kind was traced in the frame loop, only this UI exit).
  - `8` — entered from the gameplay side (script- or gameplay-driven; the screen units themselves only *read* it, `src/game/80028618.c:191`), and `gFinishScreenModule`'s arm 8 turns it into `10` when `gScreenResetFlag == 0`, spinning until the module finishes unloading (`src/game/80025B7C.c:100-114`).
  - `0xD` (13) — mission countdown expired (`gScreenResetFlag` reached 0) in session mode `unk5C == 1`: `gAdvanceGameplayScreenCountdown` stops sound track 0x27 on both players and writes `gScreenState = 0xD` (`src/game/80027D9C.c:78-88`). In 0xD the frame loop keeps submitting frames, but now each round runs `gDrawPlayerResultsScreen` (`src/game/80028618.c:174-177`), the in-game HUD no longer draws (`src/game/80047F30.c:188` gates on `== 4`), the frame callback keeps clocking (`src/game/8001EFF8.c:57` treats 4 and 13 alike) and the countdown initializer `gAdvanceGameplayScreenCountdown` is skipped (`src/game/80028618.c:181` gates on `4 || 0xD`). This is the results phase after a timed Precinct Assault match.
  - `9` / `10` / `3` — teardown-mapped values: `gFinishScreenModule`'s arm 8 writes 10 in its spin (`src/game/80025B7C.c:102`) and arm 9 writes `unk10 = (unk40 == unk44) ? 0 : 1` (`:116-120`); no writer of the *screen state* 9 itself was found in the audited sources (it is produced by code outside the screen units — see the Unproven note); state 3 appears only as the pending/boot value (`gPendingScreenState = 3`, `src/game/80076A2C.c:212,286`).
  - The pending value `gPendingScreenState` (`D_8009F9A8`) is a separate word consumed by `func_800782EC` (`:67`) before the next module; `gGameMain` copies `gGameSession->unk10` into it after each module (`src/game/80076A2C.c:280`).
  - `gScreenResetFlag` (`D_8009FA70`), the countdown that ends a timed match, is armed with `0x78` by the cheat `0x01ADCFED` (`src/game/80060A0C.c:120` context) and decremented by `gFrameDelta` in `gAdvanceGameplayScreenCountdown` (`src/game/80027D9C.c:70`).
* **The 1P HUD score attribution (`+0x68`/`unk64[1]`) — resolved.** `+0x68` is `score[1]`, the second element of the `s32 score[2]` array at session `+0x64` (`struct State` in `src/game/80047F30.c:38`), and the HUD shows `score[game->player->valuec1]` (`src/game/80047F30.c:214`). `valuec1` (player `+0xC1`, `src/game/80047F30.c:14`) is the *assigned player slot* 0/1: `gInitPlayerUnit` books the chosen slot into `gPlayerEntities[index]` and `state->index` (the `+0xC1` byte) from the spawn record's preferred index, falling back to the first free slot (`src/game/800579D4.c:176-190`). In a 1P Precinct Assault match there is only one player entity, so it books slot 0 and `valuec1 == 0` — the first HUD score box reads `score[0]`, i.e. `+0x64`. **The `unk64[1]`/`unk68` fixed-[1] writes of the 1P capture branches (`src/game/8002D1A0.c:218`, `src/game/8002D664.c:192`) are the opposite side**: they only fire when the capturing unit's team *matches* `gSinglePlayerEntity`'s team (`e->unk18 == id`, `src/game/8002D1A0.c:216`), and `gSinglePlayerEntity` is the type-37 enemy drone publish (`gSinglePlayerEntity = obj`, `src/game/800331CC.c:243`) — so `+0x68` accumulates the *enemy's* outpost claims and the 1P HUD's second score box (`score[1]`, `src/game/80047F30.c:253`) shows it beside the stage line. The two struct views (`unk64[2]` in `src/game/8002D1A0.c:74-78` vs `unk68` in `src/game/8002D664.c:55-59`) are the same array at different offsets. Which team id the *human* player holds (and therefore whether their own claims land in `score[0]` or `score[1]`) is not decided by these units; the 1P branch keys it off the drone's team byte (`unk18`) — see Unproven.
* The spawner payload is inline at the descriptor's `+0x38`; the trailing table pointer is `payload - 4`. `src/game/80060E74.c:138-144`.
* No purchase/build UI, no unit price and no cost field exists in the retail image. See *What is not in the retail code*.

## Likely

* The `D_800A0C70` byte fields are `[0]` primary spawn column, `[1]` destruction-spawn column, `[0x18..0x19]` a flag halfword (bit 0 gates the random fallback), `[0x1A]` a fallback column, `[0x04 + slot*4]` five slot columns. Basis: `func_8003B838` (`[0]`), `func_8003B7D8` (`[1]`), `func_8003B4E0` (`+0x18` bit 0, `+0x1A`, `+4+slot*4`), `func_8003BC7C` (the copy sequence). The *semantics* of those columns is inferred from their consumers, not stated anywhere.
* The `0x4000` flag path is the base's "a quarter of the capacity elapsed → produce the next unit tier" mechanism: `func_8003BA50` sets it, `func_8004CE04` clears it and calls `func_8003BAE4`, which picks a column by quarter index. Basis: `src/game/8004CE04.c:60-63` and `src/game/8003BAE4.c:62-83`. The comment in `func_8003BAE4` names the quarter index a "grid type", which conflicts with `func_8003B65C`'s actual use as a 0..3 bucket; the *mechanism* is clear, the naming is not.
* ~~`gScreenState` values `4` and `0xD` both mean "a menu is up…"~~ — **resolved and promoted to Confirmed** (see the screen-state machine bullet below): `4` is the interactive module state, `0xD` is the post-countdown results state; the world grid exists in both.
* Mode 2 (steal) scoring double the claim value is a deliberate design decision rather than an artefact, since both increments are scaled by the same mode variable and the captured-flag byte `unk23` also selects between two distinct value bytes (`unkF8`/`unkF9`).

## Unproven

* That the two team ids `unkF6`/`unkF7` are "owner" and "challenger" in a fixed order. The code only ever compares against them; nothing shows which one is the initial owner.
* The exact meaning of cell flag bits 0 and 2 in `func_800402DC`'s `(word & 5) == 4` test, and what `D_8009F31C` selects in the object constructor.
* That `func_8003BDA0`'s `'tACT'` record is the *only* writer of `D_800A0C70` entries. `func_8003BC7C` has a second likely caller `func_8003BFD0` this audit did not open.
* `func_80060E74`'s contract with the asset loader: how many spawner records a level has, and whether the loader may register the same record twice (the suite shows the append itself is unconditional).
* Whether the screen state `9` has a writer inside the audited `src/game` set: `gFinishScreenModule` has an arm for it (`src/game/80025B7C.c:115-121`) but the exhaustive writer search (`gScreenState =`, all sites) found no write of 9 — the state is presumably written by script/mission code outside these sources. What 9 *means* is likewise unlabelled in the audited code; nothing here reads a literal 9.

## Open questions

1. Who calls `func_8003BC7C` besides `func_8003BDA0`? If `func_8003BFD0` exists as a second writer, slot snapshots could be reloaded mid-level, which would change when `func_8003B838` starts returning non-zero.
2. ~~Which screen state is `0xD`?~~ **Resolved (see the Confirmed screen-state machine):** `0xD` is the post-countdown results state — countdown expired with session mode `unk5C == 1`; the frame loop keeps running, `gDrawPlayerResultsScreen` is submitted each round, and the in-game HUD stops drawing. It behaves like state 4 in `func_8003FA34` because the level's grid stays intact; nothing is torn down yet — teardown happens in `gFinishScreenModule` via the default arm (`unk10 = 4`).
3. Is the deferral stack order observable in-game? `func_8003FAD4` replays newest-first, so a base spawned later is registered before an earlier one. Whether that matters depends on whether `func_800402DC` rejects duplicate cells, which the suite does not test.
4. Does anything consume the `0x4000` flag *other* than `func_8004CE04`? If a second consumer exists, the quarter-change marker has a second effect this document does not describe.
5. Are the `unkFA`/`unkFB` score increments per-objective data (loaded per mission from the level file) or a single global tuning constant? `func_8002D1A0` reads them from the entity, so they are per-objective — but nothing in the audited code ever *writes* them, so their source is the level loader.
6. ~~`func_8002D664` message 5 increments `gGameSession->unk68`/`unk80` in single player but `+0x64`/`+0x7C` in split screen...~~ **Resolved:** `unk68` is `unk64[1]` — one `s32[2]` array at session `+0x64` (`src/game/80047F30.c:38`), seen through two struct views (`src/game/8002D1A0.c:74-78` vs `src/game/8002D664.c:55-59`). The 1P branches write the fixed index `[1]` because they fire when the claimer is on the single-player *opponent's* team (`gSinglePlayerEntity->unk18 == id`, `src/game/8002D1A0.c:216`; `gSinglePlayerEntity` is the type-37 enemy drone, `src/game/800331CC.c:243`), while the 2P branches write the claiming player's own slot `unkC1`. What remains open is only the `unk10`-vs-`unk12` pair (`players[i ^ 1]->unkAC->unk10++` in the 2P branch vs `unk12` in `func_8002D1A0:231`): both are per-player stats counters, but no reader of either was traced, so whether they are the same statistic is unresolved.
