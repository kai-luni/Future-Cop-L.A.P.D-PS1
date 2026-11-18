# Weapon cadence and projectile range

Traced from the retail code so the fire rhythm of every weapon and the leash of every projectile can
be reproduced instead of guessed. All statements name the file they come from.

## Time base

One tick is a 1/300-second unit. `func_8002703C` waits for at least five palette ticks per frame
(`do { ... } while (diff < 5)`) and caps the frame delta at `0x3C`; `func_8001EFF8:48` adds five per
VSync. So `gFrameDelta` (called `delta` below) is 5 on a 60 Hz frame, and every timer, speed and leash is
expressed in those ticks.

## Trigger logic

`func_800588D4:270-272` runs the timers before the slot update:

1. `func_8006C3E0:59-68` — for each of the three slots, `if (timer > 0) timer -= delta`, clamped at
   zero. A timer that is zero or negative is not touched, which is what lets a negative gap fire a
   burst on consecutive frames.
2. `func_8006BA50:169-171` — `if (ctl->unk78[idx] > 0) return;` — still reloading, no shot.
3. `func_8006BA50:172-185` — the shot counter: the shared `+0x8A` when it is positive (this also
   switches to the second fire half at `config + 0x24`), otherwise per-slot `+0x84`. Zero or less
   plays SE `0xD` and returns.
4. `func_8006BA50:187-197` — the definition comes from `func_8006AAEC(ctl->unk90[idx])`; types 10..12
   and 14 go to the beam update `func_8006B23C`, everything else to `func_8006ACB0`.
5. `func_8006BA50:205-210` — after a successful shot:
   `if (--ctl->unk93[idx] == 0) { ctl->unk93[idx] = e->unk06; ctl->unk78[idx] += e->unk08; }`
   `else { ctl->unk78[idx] += e->unk0A; }`
6. `func_8006BA50:211-217` — in the shared-counter case a counter that reached zero clears `+0x8A`,
   reloads `+0x93` from the record's `+0x0A` and swaps `record + 0x44` / `+0x50`, the two references
   that pick the projectile model of each half.

The three cadence fields, relative to the fire half (`config + 0x04` or `config + 0x24`):

| offset | type | meaning |
| --- | --- | --- |
| `+0x06` | u8 | shots per reload cycle; the same byte seeds the `+0x93` cycle counter |
| `+0x08` | s16 | reload timer added to `+0x78` when the cycle ends |
| `+0x0A` | s16 | timer added after a shot inside the cycle; negative values are a credit |

## The sixteen fire halves of M1A1.MIS

Values read straight out of `build/disc/files/CW/M1A1.MIS`; "frames" is the gap between two shots at
`delta = 5`, "shots/s" the resulting rate at 60 Hz.

| definition | action | burst | reload | gap | frames | shots/s |
| --- | --- | --- | --- | --- | --- | --- |
| `4Da` | 3 | 1 | 8 | 0 | 2 | 30 |
| `4Db` | 0x12 | 1 | 12 | 0 | 3 | 20 |
| `4Fa` | 2 | 4 | 24 | -2 | 7 (burst of 4) | 34.3 |
| `4Fb` | 2 | 8 | 24 | -1 | 11 (burst of 8) | 43.6 |
| `51a` | 3 | 1 | 16 | 16 | 4 | 15 |
| `51b` | 3 | 1 | 16 | 16 | 4 | 15 |
| `53a` | 0x15 | 1 | 48 | 0 | 10 | 6 |
| `53b` | 0x15 | 3 | 64 | 0 | 15 (burst of 3) | 12 |
| `54a` | 4 | 1 | 32 | 32 | 7 | 8.6 |
| `54b` | 0x14 | 1 | 16 | 16 | 4 | 15 |
| `56a` | 4 | 1 | 32 | 32 | 7 | 8.6 |
| `56b` | 0x14 | 1 | 48 | 0 | 10 | 6 |
| `57a` | 5 | 1 | 16 | 16 | 4 | 15 |
| `57b` | 5 | 1 | 16 | 0 | 4 | 15 |
| `70a` | 9 | 1 | 16 | 0 | 4 | 15 |
| `70b` | 9 | 2 | 16 | 1 | 5 (burst of 2) | 24 |

A burst fires on consecutive frames because the gap is negative or smaller than the delta; the cycle
then ends with the reload. `4Da` and `4Db` share the model 59 and differ only in their reload, which
is why the second trigger of the same weapon feels slower.

## Units and leash

`func_80010750` loads the entity translation shifted right by four and `func_80010234:46-51` stores a
transformed vertex shifted left by four, so a world position is `position + 16 * vertex`. The position
unit is therefore the finer one: **16 position units make one model unit**.

- `func_80035904:189-193` stores the spawn point in `unkA0..unkA4` as `position >> 8`, i.e. rounded
  down to a multiple of 256 position units.
- `func_80035904:234` stores `unkA6 = (arc << 4) >> 8`, and `func_80035B7C:434` ends the flight with
  `if ((self->unkA6 << 8) < s4) self->unk1C = -1;` where `s4` is the octagonal distance between the
  home point and the current position (`func_8001A4E8`: largest axis plus a quarter of the other two).
  The leash is therefore `arc << 4` rounded down to a multiple of 256 position units, and in model
  units roughly the arc word itself.
- The test runs **before** the step of the same frame, so a projectile overshoots its leash by up to
  one step - diagonally it flies further than the radius, which the octagonal metric already accounts
  for.
- The step itself is `s3 = delta * unk84` (`func_80035B7C:450`) with `unk84` seeded from
  `args + 0x2C` (`func_80035904:196`, `speed << 4` or `speed << 1`), and the ramp of
  `func_80035B7C:435-448` is `unk84 += (accel * delta) / 16` clamped at `unkD4 << 4`.
- `unkDA = 0x5DC` (`func_80035904:237`) is a fuel timer of 1500 ticks = 5 s that is only consumed in
  the homing states 1 and 3 (`func_80035B7C:459-469`); once it runs out the speed halves. Without a
  target the entity stays in state 0 and the timer is never touched, so it does not end a direct shot.

Retail consequence: a `4Da` shot with `unk84 = 131072` covers `5 * 131072` position units per frame
while its leash is 131072, so it expires in the first or second frame. Retail projectile fire is
tracer-fast; anything slower is a viewer decision, not a property of the data.

## Other ends of a shot

`unk1C = -1` is the common exit: collision with the world (`func_800684AC`), the floor test
(`func_80066F78`), a hit reported by message 11 (`func_80036624:142-155`), or reaching the target in
state 1. The same frame sends message 4 to the entity (decay/explosion, `func_8004CE04`) and the
secondary object of `def + 0x1B` spawns (`func_8006ACB0:287-296`).