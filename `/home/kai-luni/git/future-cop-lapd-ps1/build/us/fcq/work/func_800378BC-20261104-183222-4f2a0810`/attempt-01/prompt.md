# Decompilation brief — `func_800378BC` (`0x800378BC`, 280 bytes)

Repository: `/home/kai-luni/git/future-cop-lapd-ps1` — you can MEASURE there, so measure instead of guessing.
This target has **no candidate yet**: the function is open, nothing has been reconstructed.

## Acceptance rules

- A unit counts only when the project gate rebuilds **exactly** the retail bytes:
  `offline_decomp preview` prints `OK func_800378BC: <n> bytes`. A word-perfect count with a byte
  surplus is a FAIL. A zero tail behind the extent is allowed and reported.
- Absolute `j`/`jal` need the link path (`link: true`) with every called symbol; without it
  only the section offset is encoded and the word looks wrong.
- `-G` is passed per stage (cc1, maspsx, as) — a profile with `-G8` for cc1 and `-G4` for
  maspsx is valid and must not be "cleaned up".
- A jump table needs its address as `rodata_address`, and the case values follow the retail
  table index, not their own order.
- Never claim a score you did not measure.

## Levers that already decided units here

- Compiler family before C rewriting: two differing words are usually `-O` level, compiler or
  the link path (5 pinned GCCs are available).
- A register pin must not steal a register the surrounding block needs.
- Retail materialises a base address in the target register (`lui $s0` + `addiu $s0,$s0`); a
  candidate that routes it through `$v0` needs a pointer loop pinned to the retail register.
- Epilogue order (`addiu $sp` before or inside the `jr $ra` delay slot) is a compiler
  fingerprint — when only the last two words differ, change the compiler first.
- `--expand-div` reproduces checked division; `--dont-expand-li` keeps `addiu` instead of `ori`.

## Disassembly oracle (retail)

```mips
nonmatching func_800378BC, 0x118

glabel func_800378BC
    /* 280BC 800378BC D8FFBD27 */  addiu      $sp, $sp, -0x28
    /* 280C0 800378C0 1C00B1AF */  sw         $s1, 0x1C($sp)
    /* 280C4 800378C4 2188A000 */  addu       $s1, $a1, $zero
    /* 280C8 800378C8 1800B0AF */  sw         $s0, 0x18($sp)
    /* 280CC 800378CC 2180C000 */  addu       $s0, $a2, $zero
    /* 280D0 800378D0 2400BFAF */  sw         $ra, 0x24($sp)
    /* 280D4 800378D4 56AB000C */  jal        func_8002AD58
    /* 280D8 800378D8 2000B2AF */   sw        $s2, 0x20($sp)
    /* 280DC 800378DC 0800128E */  lw         $s2, 0x8($s0)
    /* 280E0 800378E0 A0002526 */  addiu      $a1, $s1, 0xA0
    /* 280E4 800378E4 9A81010C */  jal        func_80060668
    /* 280E8 800378E8 24004426 */   addiu     $a0, $s2, 0x24
    /* 280EC 800378EC 34004426 */  addiu      $a0, $s2, 0x34
    /* 280F0 800378F0 DC002526 */  addiu      $a1, $s1, 0xDC
    /* 280F4 800378F4 0400038E */  lw         $v1, 0x4($s0)
    /* 280F8 800378F8 50002226 */  addiu      $v0, $s1, 0x50
    /* 280FC 800378FC 1000A2AF */  sw         $v0, 0x10($sp)
    /* 28100 80037900 1800668C */  lw         $a2, 0x18($v1)
    /* 28104 80037904 8343010C */  jal        func_80050E0C
    /* 28108 80037908 21382002 */   addu      $a3, $s1, $zero
    /* 2810C 8003790C ADDF000C */  jal        func_80037EB4
    /* 28110 80037910 21202002 */   addu      $a0, $s1, $zero
    /* 28114 80037914 2800228E */  lw         $v0, 0x28($s1)
    /* 28118 80037918 5A0120A2 */  sb         $zero, 0x15A($s1)
    /* 2811C 8003791C 1000428C */  lw         $v0, 0x10($v0)
    /* 28120 80037920 00000000 */  nop
    /* 28124 80037924 12004010 */  beqz       $v0, .L80037970
    /* 28128 80037928 05000224 */   addiu     $v0, $zero, 0x5
    /* 2812C 8003792C 16002392 */  lbu        $v1, 0x16($s1)
    /* 28130 80037930 00000000 */  nop
    /* 28134 80037934 09006214 */  bne        $v1, $v0, .L8003795C
    /* 28138 80037938 21380000 */   addu      $a3, $zero, $zero
    /* 2813C 8003793C 4A004292 */  lbu        $v0, 0x4A($s2)
    /* 28140 80037940 00000000 */  nop
    /* 28144 80037944 580122A2 */  sb         $v0, 0x158($s1)
    /* 28148 80037948 49004292 */  lbu        $v0, 0x49($s2)
    /* 2814C 8003794C 00000000 */  nop
    /* 28150 80037950 01004234 */  ori        $v0, $v0, 0x1
    /* 28154 80037954 590122A2 */  sb         $v0, 0x159($s1)
    /* 28158 80037958 48004792 */  lbu        $a3, 0x48($s2)
  .L8003795C:
    /* 2815C 8003795C 2800228E */  lw         $v0, 0x28($s1)
    /* 28160 80037960 C8002526 */  addiu      $a1, $s1, 0xC8
    /* 28164 80037964 1000448C */  lw         $a0, 0x10($v0)
    /* 28168 80037968 6212010C */  jal        func_80044988
    /* 2816C 8003796C 01000624 */   addiu     $a2, $zero, 0x1
  .L80037970:
    /* 28170 80037970 D441000C */  jal        func_80010750
    /* 28174 80037974 2C002426 */   addiu     $a0, $s1, 0x2C
    /* 28178 80037978 5C002526 */  addiu      $a1, $s1, 0x5C
    /* 2817C 8003797C 01000624 */  addiu      $a2, $zero, 0x1
    /* 28180 80037980 70002486 */  lh         $a0, 0x70($s1)
    /* 28184 80037984 2800228E */  lw         $v0, 0x28($s1)
    /* 28188 80037988 00210400 */  sll        $a0, $a0, 4
    /* 2818C 8003798C 3400428C */  lw         $v0, 0x34($v0)
    /* 28190 80037990 08008424 */  addiu      $a0, $a0, 0x8
    /* 28194 80037994 8D40000C */  jal        func_80010234
    /* 28198 80037998 21204400 */   addu      $a0, $v0, $a0
    /* 2819C 8003799C 1801228E */  lw         $v0, 0x118($s1)
    /* 281A0 800379A0 00000000 */  nop
    /* 281A4 800379A4 00404230 */  andi       $v0, $v0, 0x4000
    /* 281A8 800379A8 03004010 */  beqz       $v0, .L800379B8
    /* 281AC 800379AC 21202002 */   addu      $a0, $s1, $zero
    /* 281B0 800379B0 538C010C */  jal        func_8006314C
    /* 281B4 800379B4 21280000 */   addu      $a1, $zero, $zero
  .L800379B8:
    /* 281B8 800379B8 2400BF8F */  lw         $ra, 0x24($sp)
    /* 281BC 800379BC 2000B28F */  lw         $s2, 0x20($sp)
    /* 281C0 800379C0 1C00B18F */  lw         $s1, 0x1C($sp)
    /* 281C4 800379C4 1800B08F */  lw         $s0, 0x18($sp)
    /* 281C8 800379C8 2800BD27 */  addiu      $sp, $sp, 0x28
    /* 281CC 800379CC 0800E003 */  jr         $ra
    /* 281D0 800379D0 00000000 */   nop
endlabel func_800378BC
```

### Neighbours of this function (5 verified, 4 open)

#### Callers

Open — no C exists for these; read the argument shape from the oracle around each `jal` (which registers are loaded right before it), never invent it:

| function | address | size |
|---|---|---|
| `func_800382B4` | `0x800382B4` | 1184 B |

#### Callees

| function | address | size |
|---|---|---|
| `func_8002AD58` | `0x8002AD58` | 252 B |
| `func_80044988` | `0x80044988` | 112 B |
| `func_80050E0C` | `0x80050E0C` | 984 B |
| `func_80060668` | `0x80060668` | 336 B |
| `func_8006314C` | `0x8006314C` | 72 B |

Open — no C exists for these; read the argument shape from the oracle around each `jal` (which registers are loaded right before it), never invent it:

| function | address | size |
|---|---|---|
| `func_80010234` | `0x80010234` | 108 B |
| `func_80010750` | `0x80010750` | 208 B |
| `func_80037EB4` | `0x80037EB4` | 376 B |

The sources of the verified neighbours that fit the size budget are printed in the *Verified neighbours* section below; the rest are described by their call sites in the oracle.

### Verified neighbours

**Verified callee `func_8002AD58`** (`0x8002AD58`, `0xFC` bytes, compiler `gcc-2.7.2-cdk`)

```c
#include "game/types.h"

struct Unk2AD58 {
    u8 unk00[0x24];
    u32 flags;
    u8 unk28[0x50];
    s32 unk78;
    u8 unk7C[4];
    s32 unk80;
};

extern void func_8002A5F4(struct Unk2AD58 *object, void *data);
extern s32 func_8002ACB4(struct Unk2AD58 *object, s32 value);
extern s32 func_8002A810(struct Unk2AD58 *object, s32 arg1, s32 arg2);
extern s32 func_8002A940(struct Unk2AD58 *object, void *data);
extern s32 func_8002AA1C(s32 message, struct Unk2AD58 *object, void *hit);
extern s32 func_8002AB8C(struct Unk2AD58 *object, void *data);
extern s32 func_80095C30(s32 x, s32 y);

s32 func_8002AD58(s32 message, struct Unk2AD58 *object, void *data)
{
    switch (message) {
    case 1:
        return 0xA0;
    case 2:
        func_8002A5F4(object, data);
        return 0;
    case 3:
        if (object->flags & 0x20) {
            return 0;
        }
        return func_8002ACB4(object, 1);
    case 4:
        return func_8002A810(object, 1, 1);
    case 5:
    case 6:
    case 7:
    case 8:
    case 13:
        return 0;
    case 9:
        return func_8002A940(object, data);
    case 11:
    case 14:
        return func_8002AA1C(message, object, data);
    case 12:
        return func_8002AB8C(object, data);
    case 10:
    case 16:
        return 1;
    case 15:
        return func_80095C30(object->unk78, object->unk80) & 0xFFF;
    default:
        return -1;
    }
}
```

**Verified callee `func_80044988`** (`0x80044988`, `0x70` bytes, compiler `gcc-2.7.2-psx`)

```c
#include "game/types.h"

struct AnimClip {
    u8 unk00[4];
    s16 start;
    s16 end;
    u8 unk08[8];
};

struct AnimTrack {
    struct AnimClip *clips;
    struct AnimClip *clip;
    s32 elapsed;
    s16 frame;
    s16 target;
    s8 step;
    u8 index;
    u8 count;
    u8 flags;
};

extern struct AnimClip *func_800448D8(struct AnimTrack *track, s32 index, s32 direction,
                                      u8 flags);

void func_80044988(u8 *data, struct AnimTrack *track, u8 forward, s32 index)
{
    if (data == 0) {
        track->clips = 0;
        return;
    }
    track->clips = (struct AnimClip *)(data + 4);
    track->count = data[0];
    func_800448D8(track, index, forward ? 1 : -1, 0);
    track->elapsed = 0;
    track->flags = 0;
}
```

**Verified callee `func_80060668`** (`0x80060668`, `0x150` bytes, compiler `gcc-2.7.2-cdk`)

```c
#include "game/types.h"

struct Unk5FF70 {
    u8  key;
    u8  pad01[1];
    u16 f02;
    u16 f04;
    u16 f06;
    u16 f08;
    u16 f0A;
    u8  pad0C[0x24];
};

struct Unk60668In {
    s16 f00;
    u16 f02;
    u8  f04;
    u8  f05;
    u16 f06;
    u16 f08;
    u16 f0A;
    u16 f0C;
    s16 f0E;
};

struct Unk60668Out {
    struct Unk5FF70 *f00;
    u16 f04;
    u8  f06;
    u8  f07;
    u16 f08;
    u16 f0A;
    s32 f0C;
    s32 f10;
    s16 f14;
    s16 f16;
    s32 f18;
    s32 f1C;
    u16 f20;
    u8  f22;
    u8  f23;
    u16 f24;
    u16 f26;
};

extern s32 func_8005FF70(s32 key);
extern s32 func_8006062C(u32 id);
extern struct Unk5FF70 D_800A2300[];

void func_80060668(struct Unk60668In *in, struct Unk60668Out *out)
{
    s32 index;
    s32 scale;
    u16 v20;

    if (in->f00 < 0) {
        out->f00 = 0;
    } else {
        index = func_8005FF70(in->f00);
        out->f00 = &D_800A2300[index];
        out->f04 = in->f02;
        out->f06 = in->f05;
        out->f08 = in->f06;
        out->f07 = in->f04;
        out->f0C = func_8006062C((s16)in->f08 / 2);
        out->f10 = func_8006062C((s16)in->f0A / 2);
        out->f0A = in->f0C;
        scale = in->f0E << 12;
        out->f14 = (s16)(scale / 218);
        out->f16 = 0;
        v20 = out->f00->f02;
        out->f18 = 0;
        out->f1C = 0;
        out->f22 = 0;
        out->f23 = 0;
        out->f20 = v20;
        out->f24 = out->f00->f0A;
        out->f26 = out->f00->f06;

    }
}
```

**Verified callee `func_8006314C`** (`0x8006314C`, `0x48` bytes, compiler `gcc-2.7.2-psx`)

```c
#include "game/types.h"

struct Unk631A0 {
    s32 key;
    u8 value;
};

extern s32 D_8009F838;
extern struct Unk631A0 D_800A31A0[];

s32 func_8006314C(s32 key, u8 value)
{
    struct Unk631A0 *entry;

    if (D_8009F838 == 0x18) {
        return 0;
    }
    entry = &D_800A31A0[D_8009F838];
    entry->value = value;
    entry->key = key;
    D_8009F838++;
    return 1;
}
```


### Shape-similar verified units

`func_80052D7C` (`0xAC` bytes):

```c
#include "game/types.h"

struct Unk52D7CModel {
    u8 unk00[0x34];
    u8 *frames;
};

struct Unk52D7CPart {
    struct Unk52D7CModel *model;
    u8 matrix[0x30];
    u8 output[0x14];
    s16 frame;
    s16 unk4A;
};

struct Unk52D7CState {
    u8 unk00[0x178];
    struct Unk52D7CPart parts[2];
};

struct Unk52D7CInfo {
    u8 unk00[0xAC];
    struct Unk52D7CState *state;
};

struct Unk52D7C {
    u8 unk00[0x28];
    u8 coord[0x8C];
    struct Unk52D7CInfo *info;
};

extern void func_80052960(u8 *coord, u8 *first, u8 *second);
extern void func_80010750(u8 *matrix);
extern void func_80010234(u8 *frame, u8 *output, s32 count);

void func_80052D7C(struct Unk52D7C *object)
{
    struct Unk52D7CState *state;
    register u8 *pointer __asm__("$2");
    register s32 offset __asm__("$4");

    state = object->info->state;
    func_80052960(object->coord, state->parts[0].matrix, state->parts[1].matrix);
    func_80010750(state->parts[0].matrix);
    offset = state->parts[0].frame;
    pointer = (u8 *)state->parts[0].model;
    offset <<= 4;
    __asm__("" : "=r"(offset) : "0"(offset));
    pointer = ((struct Unk52D7CModel *)pointer)->frames;
    offset += 8;
    func_80010234(pointer + offset, state->parts[0].output, 1);
    func_80010750(state->parts[1].matrix);
    offset = state->parts[1].frame;
    pointer = (u8 *)state->parts[1].model;
    offset <<= 4;
    __asm__("" : "=r"(offset) : "0"(offset));
    pointer = ((struct Unk52D7CModel *)pointer)->frames;
    offset += 8;
    func_80010234(pointer + offset, state->parts[1].output, 1);
}
```

`func_8002CB20` (`0x110` bytes):

```c
#include "game/types.h"

struct Model;
struct ModelInstance;

struct Command {
    u8 op;
    u8 arg;
};

struct Unk2CB20 {
    u8 unk00[0x0C];
    struct Command commands[4];
    u8 unk14[2];
    u8 unk16;
    u8 unk17[0x0D];
    u32 options;
    u8 unk28[0xB4];
    s16 unkDC;
    u8 unkDE;
    u8 unkDF;
    u8 unkE0[0x28];
    struct Model *models[4];
};

extern void func_80013AF0(void *instance, struct Model *model, u32 options);
extern void func_8002A5B4(struct Unk2CB20 *self, s32 index);

void func_8002CB20(struct Unk2CB20 *self)
{
    s32 i;
    s32 op;
    u32 arg;

    for (i = 0; i < 4;) {
        op = self->commands[i].op;
        if (op == 0) {
            break;
        }
        arg = self->commands[i].arg;
        switch (op) {
        case 0x29:
        case 0x2A:
        case 0x2B:
        case 0x2C:
            self->unkDE = op - 0x28;
            self->unkDC = arg;
            break;
        case 0x2D:
            if (arg != 0) {
                self->unkDF &= 0xFB;
            } else {
                self->unkDF |= 4;
            }
            break;
        case 0x2E:
            if (self->unk16 == 0x1E && arg < 4) {
                func_80013AF0(&self->unk28, self->models[arg], self->options);
            }
            break;
        default:
            i++;
            continue;
        }
        func_8002A5B4(self, i);
    }
}
```

`func_80055E10` (`0xD8` bytes):

```c
#include "game/types.h"

struct InputAction {
    u32 buttons;
    s32 phase;
};

struct Unk55E10Control {
    u8 unk00[0x30];
    u32 buttons;
    struct InputAction fire;
    struct InputAction special;
    u8 unk44[8];
    struct InputAction weapon;
    u8 unk54[0x51];
    u8 weaponState;
    u8 unkA6[0x26A];
    s8 locked;
};

struct Unk55E10 {
    u8 unk00[0xAC];
    struct Unk55E10Control *control;
};

extern void func_8005780C(u32 buttons, struct InputAction *action);
extern void func_8006C34C(struct Unk55E10 *object, s32 arg1);
extern void func_8006BA50(struct Unk55E10 *object, s32 arg1);

void func_80055E10(struct Unk55E10 *object)
{
    struct Unk55E10Control *control;
    u32 buttons;

    control = object->control;
    if (control->locked != 0) {
        return;
    }
    buttons = control->buttons;
    func_8005780C(buttons, &control->special);
    func_8005780C(control->buttons, &control->fire);
    func_8005780C(control->buttons, &control->weapon);
    if (control->fire.phase == 0) {
        func_8006C34C(object, 0);
    }
    if (buttons & 0x4000000) {
        func_8006BA50(object, 0);
    }
    if (buttons & 0x400000) {
        func_8006BA50(object, 1);
    }
    if (control->weapon.phase == control->weaponState) {
        func_8006BA50(object, 2);
    }
}
```


## Answer format

Deliver one markdown file with a ```json metadata block (`schema: fcq/answer-v1`, target,
run_id, model, status `exact|partial`, `source_sha256`, words, bytes, compiler,
`compiler_flags_extra`, `maspsx_flags_extra`, `link`, `gate_status`, `note`) and exactly one
```c block with the complete source that was measured. Then publish it through the run's
`fcq publish` command.
