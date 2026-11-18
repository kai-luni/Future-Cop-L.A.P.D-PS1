# Decompilation brief — `func_8005B5A0` (`0x8005B5A0`, 264 bytes)

Repository: `/home/kai-luni/git/future-cop-lapd-ps1` — you can MEASURE there, so measure instead of guessing.
This target has **no candidate yet**: the function is open, nothing has been reconstructed.

## Acceptance rules

- A unit counts only when the project gate rebuilds **exactly** the retail bytes:
  `offline_decomp preview` prints `OK func_8005B5A0: <n> bytes`. A word-perfect count with a byte
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
nonmatching func_8005B5A0, 0x108

glabel func_8005B5A0
    /* 4BDA0 8005B5A0 C0FFBD27 */  addiu      $sp, $sp, -0x40
    /* 4BDA4 8005B5A4 3800B2AF */  sw         $s2, 0x38($sp)
    /* 4BDA8 8005B5A8 21908000 */  addu       $s2, $a0, $zero
    /* 4BDAC 8005B5AC 3000B0AF */  sw         $s0, 0x30($sp)
    /* 4BDB0 8005B5B0 2180A000 */  addu       $s0, $a1, $zero
    /* 4BDB4 8005B5B4 3400B1AF */  sw         $s1, 0x34($sp)
    /* 4BDB8 8005B5B8 2188C000 */  addu       $s1, $a2, $zero
    /* 4BDBC 8005B5BC 33000012 */  beqz       $s0, .L8005B68C
    /* 4BDC0 8005B5C0 3C00BFAF */   sw        $ra, 0x3C($sp)
    /* 4BDC4 8005B5C4 0400228E */  lw         $v0, 0x4($s1)
    /* 4BDC8 8005B5C8 5C00038E */  lw         $v1, 0x5C($s0)
    /* 4BDCC 8005B5CC 00000000 */  nop
    /* 4BDD0 8005B5D0 23104300 */  subu       $v0, $v0, $v1
    /* 4BDD4 8005B5D4 1000A2AF */  sw         $v0, 0x10($sp)
    /* 4BDD8 8005B5D8 0800228E */  lw         $v0, 0x8($s1)
    /* 4BDDC 8005B5DC 6000038E */  lw         $v1, 0x60($s0)
    /* 4BDE0 8005B5E0 00000000 */  nop
    /* 4BDE4 8005B5E4 23104300 */  subu       $v0, $v0, $v1
    /* 4BDE8 8005B5E8 1400A2AF */  sw         $v0, 0x14($sp)
    /* 4BDEC 8005B5EC 0C00228E */  lw         $v0, 0xC($s1)
    /* 4BDF0 8005B5F0 6400038E */  lw         $v1, 0x64($s0)
    /* 4BDF4 8005B5F4 1000A427 */  addiu      $a0, $sp, 0x10
    /* 4BDF8 8005B5F8 23104300 */  subu       $v0, $v0, $v1
    /* 4BDFC 8005B5FC F367000C */  jal        func_80019FCC
    /* 4BE00 8005B600 1800A2AF */   sw        $v0, 0x18($sp)
    /* 4BE04 8005B604 10002486 */  lh         $a0, 0x10($s1)
    /* 4BE08 8005B608 6E000386 */  lh         $v1, 0x6E($s0)
    /* 4BE0C 8005B60C 00000000 */  nop
    /* 4BE10 8005B610 21208300 */  addu       $a0, $a0, $v1
    /* 4BE14 8005B614 18008400 */  mult       $a0, $a0
    /* 4BE18 8005B618 12400000 */  mflo       $t0
    /* 4BE1C 8005B61C 03220800 */  sra        $a0, $t0, 8
    /* 4BE20 8005B620 2A104400 */  slt        $v0, $v0, $a0
    /* 4BE24 8005B624 19004010 */  beqz       $v0, .L8005B68C
    /* 4BE28 8005B628 02000224 */   addiu     $v0, $zero, 0x2
    /* 4BE2C 8005B62C 2600A2A3 */  sb         $v0, 0x26($sp)
    /* 4BE30 8005B630 1C004296 */  lhu        $v0, 0x1C($s2)
    /* 4BE34 8005B634 00000000 */  nop
    /* 4BE38 8005B638 2400A2A7 */  sh         $v0, 0x24($sp)
    /* 4BE3C 8005B63C 0800428E */  lw         $v0, 0x8($s2)
    /* 4BE40 8005B640 0E000424 */  addiu      $a0, $zero, 0xE
    /* 4BE44 8005B644 14004294 */  lhu        $v0, 0x14($v0)
    /* 4BE48 8005B648 21280002 */  addu       $a1, $s0, $zero
    /* 4BE4C 8005B64C 2000A2AF */  sw         $v0, 0x20($sp)
    /* 4BE50 8005B650 0800428E */  lw         $v0, 0x8($s2)
    /* 4BE54 8005B654 2000A627 */  addiu      $a2, $sp, 0x20
    /* 4BE58 8005B658 2800A2AF */  sw         $v0, 0x28($sp)
    /* 4BE5C 8005B65C 0400A28C */  lw         $v0, 0x4($a1)
    /* 4BE60 8005B660 00000000 */  nop
    /* 4BE64 8005B664 09F84000 */  jalr       $v0
    /* 4BE68 8005B668 21380000 */   addu      $a3, $zero, $zero
    /* 4BE6C 8005B66C 0A000224 */  addiu      $v0, $zero, 0xA
    /* 4BE70 8005B670 120022A2 */  sb         $v0, 0x12($s1)
    /* 4BE74 8005B674 2A004392 */  lbu        $v1, 0x2A($s2)
    /* 4BE78 8005B678 00000000 */  nop
    /* 4BE7C 8005B67C 04006230 */  andi       $v0, $v1, 0x4
    /* 4BE80 8005B680 02004010 */  beqz       $v0, .L8005B68C
    /* 4BE84 8005B684 08006234 */   ori       $v0, $v1, 0x8
    /* 4BE88 8005B688 2A0042A2 */  sb         $v0, 0x2A($s2)
  .L8005B68C:
    /* 4BE8C 8005B68C 3C00BF8F */  lw         $ra, 0x3C($sp)
    /* 4BE90 8005B690 3800B28F */  lw         $s2, 0x38($sp)
    /* 4BE94 8005B694 3400B18F */  lw         $s1, 0x34($sp)
    /* 4BE98 8005B698 3000B08F */  lw         $s0, 0x30($sp)
    /* 4BE9C 8005B69C 4000BD27 */  addiu      $sp, $sp, 0x40
    /* 4BEA0 8005B6A0 0800E003 */  jr         $ra
    /* 4BEA4 8005B6A4 00000000 */   nop
endlabel func_8005B5A0
```

### Neighbours of this function (1 verified, 1 open)

#### Callers

Open — no C exists for these; read the argument shape from the oracle around each `jal` (which registers are loaded right before it), never invent it:

| function | address | size |
|---|---|---|
| `func_8005B6A8` | `0x8005B6A8` | 1580 B |

#### Callees

| function | address | size |
|---|---|---|
| `func_80019FCC` | `0x80019FCC` | 100 B |

The sources of the verified neighbours that fit the size budget are printed in the *Verified neighbours* section below; the rest are described by their call sites in the oracle.

### Verified neighbours

**Verified callee `func_80019FCC`** (`0x80019FCC`, `0x64` bytes, compiler `gcc-2.8.1-psx`)

```c
#include "game/types.h"

/* Handwritten retail routine: returns sum(((s64)v[i] * v[i]) >> 16) for i = 0..2. */
s32 func_80019FCC(s32 *arg0)
{
    __asm__ volatile(
        "lw $13, 0x0($4)\n\t"
        "lw $14, 0x4($4)\n\t"
        "lw $15, 0x8($4)\n\t"
        "mult $13, $13\n\t"
        "mflo $8\n\t"
        "mfhi $9\n\t"
        "sll $10, $9, 16\n\t"
        "srl $11, $8, 16\n\t"
        "or $2, $10, $11\n\t"
        "mult $14, $14\n\t"
        "mflo $8\n\t"
        "mfhi $9\n\t"
        "sll $10, $9, 16\n\t"
        "srl $11, $8, 16\n\t"
        "or $12, $10, $11\n\t"
        "mult $15, $15\n\t"
        "mflo $8\n\t"
        "mfhi $9\n\t"
        "sll $10, $9, 16\n\t"
        "srl $11, $8, 16\n\t"
        "or $13, $10, $11\n\t"
        "add $2, $2, $12\n\t"
        "add $2, $2, $13"
        :
        :
        : "$2", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "memory");
}
```


### Shape-similar verified units

`func_800178DC` (`0xB8` bytes):

```c
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Camera {
    u8 unk00[0x28];
    u8 matrix[0x54];
    struct Vec3s32 eye;
};

struct Unk178DC {
    u8 unk00[0x24];
    struct Vec3s32 translation;
};

extern struct Camera *D_8009F9E4;

extern void func_80010820(u8 *matrix);
extern void func_8001006C(struct Unk178DC *object, s32 arg1, s32 count);

void func_800178DC(struct Unk178DC *object, s32 arg1)
{
    struct Vec3s32 saved;
    struct Camera *camera;

    saved = object->translation;
    camera = D_8009F9E4;
    object->translation.x -= camera->eye.x;
    object->translation.y -= camera->eye.y;
    object->translation.z -= camera->eye.z;
    func_80010820(camera->matrix);
    func_8001006C(object, arg1, 4);
    object->translation = saved;
}
```

`func_8003CF6C` (`0x140` bytes):

```c
#include "game/types.h"

struct Obj8003CF6C {
    u8 unk00[0x28];
    s32 x;
    s32 y;
    s32 z;
    u8 unk34[0xC];
    s32 unk40;
    u8 unk44[6];
    u8 flags4A;
};

struct Node8003CF6C {
    struct Node8003CF6C *next;
    u8 unk04[0x30];
    struct Obj8003CF6C *obj;
    u8 unk38;
    u8 flags39;
    u8 unk3A[6];
    u16 unk40;
    u8 unk42;
};

extern struct Node8003CF6C *D_8009F320;

extern s32 func_8001A100(s32 *vector);
extern void func_80013AA0(s32 low, s32 high);
extern u8 func_80017508(struct Obj8003CF6C *instance, u32 *flags, s32 arg2);
extern void func_800164F4(struct Obj8003CF6C *obj);

void func_8003CF6C(s32 arg0)
{
    struct Node8003CF6C *node;
    s32 vec[3];
    u32 flags;
    s32 *origin;

    origin = (s32 *)arg0;
    flags = 0;
    for (node = D_8009F320; node != 0; node = node->next) {
        if (node->obj != 0 && (node->flags39 & 2)) {
            vec[0] = node->obj->x - origin[0];
            vec[1] = node->obj->y - origin[1];
            vec[2] = node->obj->z - origin[2];
            if (func_8001A100(vec) - node->obj->unk40 > 0x1FFFF) {
                continue;
            }
            if (node->unk40 != 0) {
                func_80013AA0(((u8 *)node)[0x40], ((u8 *)node)[0x41]);
            }
            func_80017508(node->obj, &flags, node->unk42);
            if (node->unk40 != 0) {
                func_80013AA0(0, 0);
            }
            if (node->obj->flags4A & 0x10) {
                func_800164F4(node->obj);
            }
        }
    }
}
```

`func_80049100` (`0xF8` bytes):

```c
#include "game/types.h"

struct DrawTPage {
    u32 addr : 24;
    u32 len : 8;
    u32 code;
};

extern u8 *D_8009FA90[];
extern u8 *D_8009F9C0[];
extern u16 D_800A1940[];
extern u16 D_8009F768;
extern s32 D_8009F76C;

extern s32 func_80048F98(s32 character, s32 x, s32 y, u16 color);

void func_80049100(u8 *text, s32 x, s32 y, s32 color)
{
    struct DrawTPage *packet;
    u32 character;

    if (D_8009F768 == 0) {
        return;
    }
    packet = (struct DrawTPage *)D_8009FA90[0];
    if (D_8009FA90[0] < D_8009F9C0[0]) {
        D_8009FA90[0] = (u8 *)packet + 0x1C;
    }
    packet->addr = 0;
    packet->len = 1;
    packet->code = (D_800A1940[D_8009F76C] & 0x9FF) | 0xE1000000;
    while ((character = *text) != 0) {
        text++;
        if (character - 10 < 0x16) {
            color = character - 10;
        } else {
            x += func_80048F98(character, x, y, color);
        }
    }
}
```


## Answer format

Deliver one markdown file with a ```json metadata block (`schema: fcq/answer-v1`, target,
run_id, model, status `exact|partial`, `source_sha256`, words, bytes, compiler,
`compiler_flags_extra`, `maspsx_flags_extra`, `link`, `gate_status`, `note`) and exactly one
```c block with the complete source that was measured. Then publish it through the run's
`fcq publish` command.


## Previous attempt: 60/66 words, 257/264 bytes, emitted 272
Profile: {"compiler": "gcc-2.7.2-cdk", "compiler_flags_extra": ["-G4", "-O2"], "link": true, "maspsx_flags_extra": [], "rodata_address": null}

### Instructions that differ (retail vs ours)
```
0x8005B604  retail: 10002486  ours:   10002386
0x8005B608  retail: 6E000386  ours:   6E000486
0x8005B610  retail: 21208300  ours:   21186400
0x8005B614  retail: 18008400  ours:   18006300
0x8005B61C  retail: 03220800  ours:   031A0800
0x8005B620  retail: 2A104400  ours:   2A104300
```

### The C that produced this score
```c
#include "game/types.h"

struct B5A0_B {
    u8 unk00[4];
    void (*unk04)(s32, struct B5A0_B *, void *, s32);
    u8 unk08[0x54];
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    u8 unk68[6];
    s16 unk6E;
};

struct B5A0_C {
    u8 unk00[4];
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s16 unk10;
    u8 unk12;
};

struct B5A0_A {
    u8 unk00[8];
    u8 *unk08;
    u8 unk0C[0x10];
    u16 unk1C;
    u8 unk1E[0xC];
    u8 flags2A;
};

struct B5A0_packet {
    s32 unk00;
    u16 unk04;
    u8 unk06;
    u8 pad;
    u8 *unk08;
};

extern s32 func_80019FCC(s32 *vec);

void func_8005B5A0(struct B5A0_A *a0, struct B5A0_B *a1, struct B5A0_C *a2)
{
    s32 vec[3];
    struct B5A0_packet packet;
    s32 dist;
    s32 x;
    s32 y;
    s32 sum;

    if (a1 == 0) {
        return;
    }
    vec[0] = a2->unk04 - a1->unk5C;
    vec[1] = a2->unk08 - a1->unk60;
    vec[2] = a2->unk0C - a1->unk64;
    dist = func_80019FCC(vec);
    x = a2->unk10;
    y = a1->unk6E;
    sum = x + y;
    if (dist < ((sum * sum) >> 8)) {
        packet.unk06 = 2;
        packet.unk04 = a0->unk1C;
        packet.unk00 = *(u16 *)(a0->unk08 + 0x14);
        packet.unk08 = a0->unk08;
        a1->unk04(0xE, a1, &packet, 0);
        a2->unk12 = 0xA;
        if (a0->flags2A & 4) {
            a0->flags2A |= 8;
        }
    }
}
```
