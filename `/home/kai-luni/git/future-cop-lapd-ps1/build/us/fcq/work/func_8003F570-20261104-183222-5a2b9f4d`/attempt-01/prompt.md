# Decompilation brief — `func_8003F570` (`0x8003F570`, 372 bytes)

Repository: `/home/kai-luni/git/future-cop-lapd-ps1` — you can MEASURE there, so measure instead of guessing.
This target has **no candidate yet**: the function is open, nothing has been reconstructed.

## Acceptance rules

- A unit counts only when the project gate rebuilds **exactly** the retail bytes:
  `offline_decomp preview` prints `OK func_8003F570: <n> bytes`. A word-perfect count with a byte
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
nonmatching func_8003F570, 0x174

glabel func_8003F570
    /* 2FD70 8003F570 D0FFBD27 */  addiu      $sp, $sp, -0x30
    /* 2FD74 8003F574 2400B5AF */  sw         $s5, 0x24($sp)
    /* 2FD78 8003F578 21A88000 */  addu       $s5, $a0, $zero
    /* 2FD7C 8003F57C 2C00BFAF */  sw         $ra, 0x2C($sp)
    /* 2FD80 8003F580 2800B6AF */  sw         $s6, 0x28($sp)
    /* 2FD84 8003F584 2000B4AF */  sw         $s4, 0x20($sp)
    /* 2FD88 8003F588 1C00B3AF */  sw         $s3, 0x1C($sp)
    /* 2FD8C 8003F58C 1800B2AF */  sw         $s2, 0x18($sp)
    /* 2FD90 8003F590 1400B1AF */  sw         $s1, 0x14($sp)
    /* 2FD94 8003F594 2B78010C */  jal        func_8005E0AC
    /* 2FD98 8003F598 1000B0AF */   sw        $s0, 0x10($sp)
    /* 2FD9C 8003F59C 90F4000C */  jal        func_8003D240
    /* 2FDA0 8003F5A0 00000000 */   nop
    /* 2FDA4 8003F5A4 0A80023C */  lui        $v0, %hi(D_800A1070)
    /* 2FDA8 8003F5A8 70105624 */  addiu      $s6, $v0, %lo(D_800A1070)
    /* 2FDAC 8003F5AC 2190C002 */  addu       $s2, $s6, $zero
  .L8003F5B0:
    /* 2FDB0 8003F5B0 0000508E */  lw         $s0, 0x0($s2)
    /* 2FDB4 8003F5B4 00000000 */  nop
    /* 2FDB8 8003F5B8 1E000012 */  beqz       $s0, .L8003F634
    /* 2FDBC 8003F5BC 21880000 */   addu      $s1, $zero, $zero
    /* 2FDC0 8003F5C0 21A04002 */  addu       $s4, $s2, $zero
    /* 2FDC4 8003F5C4 21200002 */  addu       $a0, $s0, $zero
  .L8003F5C8:
    /* 2FDC8 8003F5C8 0000138E */  lw         $s3, 0x0($s0)
    /* 2FDCC 8003F5CC ABFC000C */  jal        func_8003F2AC
    /* 2FDD0 8003F5D0 2128A002 */   addu      $a1, $s5, $zero
    /* 2FDD4 8003F5D4 FF004230 */  andi       $v0, $v0, 0xFF
    /* 2FDD8 8003F5D8 12004010 */  beqz       $v0, .L8003F624
    /* 2FDDC 8003F5DC 00000000 */   nop
    /* 2FDE0 8003F5E0 04002012 */  beqz       $s1, .L8003F5F4
    /* 2FDE4 8003F5E4 00000000 */   nop
    /* 2FDE8 8003F5E8 0000028E */  lw         $v0, 0x0($s0)
    /* 2FDEC 8003F5EC 80FD0008 */  j          .L8003F600
    /* 2FDF0 8003F5F0 000022AE */   sw        $v0, 0x0($s1)
  .L8003F5F4:
    /* 2FDF4 8003F5F4 0000028E */  lw         $v0, 0x0($s0)
    /* 2FDF8 8003F5F8 00000000 */  nop
    /* 2FDFC 8003F5FC 000082AE */  sw         $v0, 0x0($s4)
  .L8003F600:
    /* 2FE00 8003F600 0A000424 */  addiu      $a0, $zero, 0xA
    /* 2FE04 8003F604 21280002 */  addu       $a1, $s0, $zero
    /* 2FE08 8003F608 21300000 */  addu       $a2, $zero, $zero
    /* 2FE0C 8003F60C 2400A28C */  lw         $v0, 0x24($a1)
    /* 2FE10 8003F610 2138C000 */  addu       $a3, $a2, $zero
    /* 2FE14 8003F614 09F84000 */  jalr       $v0
    /* 2FE18 8003F618 0000A0AC */   sw        $zero, 0x0($a1)
    /* 2FE1C 8003F61C 8BFD0008 */  j          .L8003F62C
    /* 2FE20 8003F620 21806002 */   addu      $s0, $s3, $zero
  .L8003F624:
    /* 2FE24 8003F624 21880002 */  addu       $s1, $s0, $zero
    /* 2FE28 8003F628 21806002 */  addu       $s0, $s3, $zero
  .L8003F62C:
    /* 2FE2C 8003F62C E6FF0016 */  bnez       $s0, .L8003F5C8
    /* 2FE30 8003F630 21200002 */   addu      $a0, $s0, $zero
  .L8003F634:
    /* 2FE34 8003F634 04005226 */  addiu      $s2, $s2, 0x4
    /* 2FE38 8003F638 1C00C226 */  addiu      $v0, $s6, 0x1C
    /* 2FE3C 8003F63C 2A104202 */  slt        $v0, $s2, $v0
    /* 2FE40 8003F640 DBFF4014 */  bnez       $v0, .L8003F5B0
    /* 2FE44 8003F644 00000000 */   nop
    /* 2FE48 8003F648 4CFD000C */  jal        func_8003F530
    /* 2FE4C 8003F64C 2120A002 */   addu      $a0, $s5, $zero
    /* 2FE50 8003F650 0A80103C */  lui        $s0, %hi(D_800A1070)
    /* 2FE54 8003F654 70101026 */  addiu      $s0, $s0, %lo(D_800A1070)
    /* 2FE58 8003F658 1000048E */  lw         $a0, 0x10($s0)
    /* 2FE5C 8003F65C AE30010C */  jal        func_8004C2B8
    /* 2FE60 8003F660 00000000 */   nop
    /* 2FE64 8003F664 1400048E */  lw         $a0, 0x14($s0)
    /* 2FE68 8003F668 0AFD000C */  jal        func_8003F428
    /* 2FE6C 8003F66C 2128A002 */   addu      $a1, $s5, $zero
    /* 2FE70 8003F670 F800828F */  lw         $v0, 0xF8($gp)
    /* 2FE74 8003F674 00000000 */  nop
    /* 2FE78 8003F678 03004010 */  beqz       $v0, .L8003F688
    /* 2FE7C 8003F67C 00000000 */   nop
    /* 2FE80 8003F680 DFFF000C */  jal        func_8003FF7C
    /* 2FE84 8003F684 00000000 */   nop
  .L8003F688:
    /* 2FE88 8003F688 0001828F */  lw         $v0, 0x100($gp)
    /* 2FE8C 8003F68C 00000000 */  nop
    /* 2FE90 8003F690 09004010 */  beqz       $v0, .L8003F6B8
    /* 2FE94 8003F694 00000000 */   nop
  .L8003F698:
    /* 2FE98 8003F698 0001848F */  lw         $a0, 0x100($gp)
    /* 2FE9C 8003F69C 00000000 */  nop
    /* 2FEA0 8003F6A0 0000908C */  lw         $s0, 0x0($a0)
    /* 2FEA4 8003F6A4 13ED000C */  jal        func_8003B44C
    /* 2FEA8 8003F6A8 00000000 */   nop
    /* 2FEAC 8003F6AC 000190AF */  sw         $s0, 0x100($gp)
    /* 2FEB0 8003F6B0 F9FF0016 */  bnez       $s0, .L8003F698
    /* 2FEB4 8003F6B4 00000000 */   nop
  .L8003F6B8:
    /* 2FEB8 8003F6B8 2C00BF8F */  lw         $ra, 0x2C($sp)
    /* 2FEBC 8003F6BC 2800B68F */  lw         $s6, 0x28($sp)
    /* 2FEC0 8003F6C0 2400B58F */  lw         $s5, 0x24($sp)
    /* 2FEC4 8003F6C4 2000B48F */  lw         $s4, 0x20($sp)
    /* 2FEC8 8003F6C8 1C00B38F */  lw         $s3, 0x1C($sp)
    /* 2FECC 8003F6CC 1800B28F */  lw         $s2, 0x18($sp)
    /* 2FED0 8003F6D0 1400B18F */  lw         $s1, 0x14($sp)
    /* 2FED4 8003F6D4 1000B08F */  lw         $s0, 0x10($sp)
    /* 2FED8 8003F6D8 3000BD27 */  addiu      $sp, $sp, 0x30
    /* 2FEDC 8003F6DC 0800E003 */  jr         $ra
    /* 2FEE0 8003F6E0 00000000 */   nop
endlabel func_8003F570
```

### Neighbours of this function (5 verified, 5 open)

#### Callers

Open — no C exists for these; read the argument shape from the oracle around each `jal` (which registers are loaded right before it), never invent it:

| function | address | size |
|---|---|---|
| `func_800271A8` | `0x800271A8` | 872 B |
| `func_80027510` | `0x80027510` | 2144 B |

#### Callees

| function | address | size |
|---|---|---|
| `func_8003F2AC` | `0x8003F2AC` | 380 B |
| `func_8003F428` | `0x8003F428` | 120 B |
| `func_8003F530` | `0x8003F530` | 64 B |
| `func_8003FF7C` | `0x8003FF7C` | 864 B |
| `func_8005E0AC` | `0x8005E0AC` | 180 B |

Open — no C exists for these; read the argument shape from the oracle around each `jal` (which registers are loaded right before it), never invent it:

| function | address | size |
|---|---|---|
| `func_8003B44C` | `0x8003B44C` | 148 B |
| `func_8003D240` | `0x8003D240` | 1088 B |
| `func_8004C2B8` | `0x8004C2B8` | 784 B |

The sources of the verified neighbours that fit the size budget are printed in the *Verified neighbours* section below; the rest are described by their call sites in the oracle.

### Verified neighbours

**Verified callee `func_8003F2AC`** (`0x8003F2AC`, `0x17C` bytes, compiler `gcc-2.7.2-cdk`)

```c
#include "game/types.h"

struct Unk3F2AC {
    u8 unk00[4];
    s16 unk04;
    u8 unk06;
    s8 unk07;
    u8 unk08[7];
    u8 unk0F;
    u8 unk10;
    u8 unk11[3];
    s16 unk14;
    u8 unk16[2];
    u16 unk18;
    u16 unk1A;
    u8 *unk1C;
    u8 *unk20;
    u8 unk24[0x48];
    void *unk6C;
};

extern s32 func_8003CAE4(struct Unk3F2AC *object, s32 arg, u8 *flag);
extern struct Unk3F2AC *func_8003E984(void *handle, struct Unk3F2AC *object,
                                      s32 flag, s32 arg3, s32 arg4);
extern void func_800498F0(void *object);

s32 func_8003F2AC(struct Unk3F2AC *object, s32 arg1)
{
    u8 flag;
    s32 result;
    struct Unk3F2AC *child;

    flag = 0;
    if (!(object->unk0F & 1)) {
        if ((object->unk0F & 8) &&
            (object->unk07 == -1 || object->unk04 <= 0)) {
            object->unk0F |= 1;
        }
        result = func_8003CAE4(object, arg1, &flag);
        if (flag != 0) {
            child = func_8003E984(object->unk1C, object, flag, 0,
                                  (object->unk10 >> 1) & 1);
            if (child != 0) {
                child->unk18 = object->unk18;
                if (result & 0xFF) {
                    child->unk07 = -1;
                    child->unk04 = -1;
                    child->unk14 = 0;
                    child->unk0F |= 2;
                    child->unk1A &= 0xF7FF;
                }
            }
        }
    }
    if (object->unk0F & 1) {
        if (object->unk07 >= 0 || object->unk04 > 0) {
            object->unk0F = (object->unk0F & ~1) | 8;
            if (object->unk20[4] == 0x5D) {
                if (object->unk6C != 0) {
                    func_800498F0(object->unk6C);
                    object->unk6C = 0;
                }
            }
        }
    }
    return object->unk0F & 1;
}
```

**Verified callee `func_8003F428`** (`0x8003F428`, `0x78` bytes, compiler `gcc-2.7.2-psx`)

```c
#include "game/types.h"

struct Unk3F428Node;

struct Unk3F428Ops {
    s32 unk00;
    void (*handler)(s32 message, struct Unk3F428Node *node, s32 arg2, s32 arg3);
};

struct Unk3F428Def {
    u8 unk00[0x48];
    struct Unk3F428Ops *ops;
};

struct Unk3F428Node {
    struct Unk3F428Node *next;
    u8 unk04[0x18];
    struct Unk3F428Def *def;
};

void func_8003F428(struct Unk3F428Node *node, s32 arg)
{
    while (node != 0) {
        if (node->def != 0 && node->def->ops != 0) {
            node->def->ops->handler(1, node, arg, 0);
        }
        node = node->next;
    }
}
```

**Verified callee `func_8003F530`** (`0x8003F530`, `0x40` bytes, compiler `gcc-2.8.1-psx`)

```c
#include "game/types.h"

extern u32 D_8009FAD8;
extern void func_8003F4A0(void);

void func_8003F530(void)
{
    __asm__ volatile(
        "addiu $sp, $sp, -0x4\n\t"
        "sw $ra, 0x0($sp)\n\t"
        "lui $at, %hi(D_8009FAD8)\n\t"
        "sw $sp, %lo(D_8009FAD8)($at)\n\t"
        "nop\n\t"
        "lui $sp, 0x1F80\n\t"
        "ori $sp, $sp, 0x3F8\n\t"
        "jal func_8003F4A0\n\t"
        "lui $sp, %hi(D_8009FAD8)\n\t"
        "lw $sp, %lo(D_8009FAD8)($sp)\n\t"
        "nop\n\t"
        "lw $ra, 0x0($sp)\n\t"
        "addiu $sp, $sp, 0x4\n\t"
    );
}
```

**Verified callee `func_8005E0AC`** (`0x8005E0AC`, `0xB4` bytes, compiler `gcc-2.7.2-cdk`)

```c
#include "game/types.h"

struct Pool {
    void *storage;
    void *active;
    void *free;
};

extern struct Pool D_800A22A0;
extern struct Pool D_800A22B0;
extern void *D_800A22F0[];

extern void func_8005B3D4(void *object);
extern void func_8005BD20(struct Pool *pool, void *object);
extern void func_8005C0C0(struct Pool *pool, void *object);
extern u8 func_8005D8F4(void *effect, void *object);
extern void func_8005E074(void *effect);

void func_8005E0AC(void *object)
{
    s32 i;

    func_8005B3D4(object);
    func_8005BD20(&D_800A22A0, object);
    func_8005C0C0(&D_800A22B0, object);
    for (i = 0; i < 4; i++) {
        if (D_800A22F0[i] != 0 && func_8005D8F4(D_800A22F0[i], object) != 1) {
            func_8005E074(D_800A22F0[i]);
            D_800A22F0[i] = 0;
        }
    }
}
```


### Shape-similar verified units

`func_8006F170` (`0xAC` bytes):

```c
#include "game/types.h"

struct PoolNode {
    struct PoolNode *next;
};

struct Pool {
    void *storage;
    struct PoolNode *active;
    struct PoolNode *free;
};

extern struct Pool D_800A3538;
extern u8 func_8006EE14(struct PoolNode *node, void *arg);
extern void func_8003BF1C(struct Pool *pool, struct PoolNode *previous,
                          struct PoolNode *node);

void func_8006F170(void *arg)
{
    struct PoolNode *node;
    struct PoolNode *next;
    struct PoolNode *previous;

    previous = 0;
    node = D_800A3538.active;
    while (node != 0) {
        next = node->next;
        if (func_8006EE14(node, arg) != 1) {
            func_8003BF1C(&D_800A3538, previous, node);
        } else {
            previous = node;
        }
        node = next;
    }
}
```

`func_8004A924` (`0x16C` bytes):

```c
#include "game/types.h"

typedef struct Node {
    s32 unk0;
    s32 unk4;
    u8 unk8[0xA];
    s16 unk12;
    u8 unk14[0x2C];
    struct Node *next;
} Node;

typedef struct MaskEntry {
    s32 mask;
    u8 pad[12];
} MaskEntry;

extern Node **D_8009F780;
extern s32 D_8009F78C;
extern s32 D_8009F794;
extern s32 D_8009FAA4;

extern void func_800497A8(void);
extern void func_8004A048(Node *node, s32 arg1, s32 arg2);

void func_8004A924(s32 arg0, s32 arg1, MaskEntry *arg2)
{
    s32 i;
    s32 j;
    s32 mask;
    Node *node;
    MaskEntry *entry;

    if (D_8009F780 == 0) {
        return;
    }

    if (D_8009F794 + 0x4B0 < D_8009FAA4) {
        func_800497A8();
        D_8009F794 = D_8009FAA4;
    }

    entry = arg2;
    for (j = 0; j < 5; j++) {
        for (i = 0; i < 5; i++) {
            mask = entry->mask;
            entry++;
            if (mask == 0) {
                continue;
            }
            node = D_8009F780[(arg0 + i) + D_8009F78C * (arg1 + j)];
            if (node == 0) {
                continue;
            }
            do {
                if (node->unk4 < 0 || node->unk4 > D_8009FAA4) {
                    if (mask & node->unk12) {
                        func_8004A048(node, arg0 + i, arg1 + j);
                    }
                }
                node = node->next;
            } while (node != 0);
        }
    }
}
```

`func_80056634` (`0x158` bytes):

```c
#include "game/types.h"

typedef struct Vec56634 {
    s32 x;
    s32 y;
    s32 z;
} Vec56634;

typedef struct Entry56634 {
    s16 f0;
    s16 f2;
    s32 f4;
    Vec56634 pos;
} Entry56634;

typedef struct Info56634 {
    u8 unk00[0xA6];
    u8 flagA6;
    u8 flagA7;
    s16 valA8;
    s16 valAA;
    s8 idAC;
    u8 unkAD[0x0F];
    s32 valBC;
} Info56634;

typedef struct Object56634 {
    u8 unk00[0xAC];
    Info56634 *info;
} Object56634;

typedef struct Player56634 {
    u8 unk00[0x50];
    Vec56634 pos;
    u8 unk5C[0x92];
    s16 angle;
    u8 unkF0[0x2E];
    s8 index;
} Player56634;

extern Player56634 *D_8009F9A0;
extern Entry56634 D_8009ACF4[];
extern Object56634 *D_8009FA58[2];

extern void func_8005612C(Vec56634 *pos, s32 angle, s32 angle2, s32 pitch, s32 dist,
                          Vec56634 *src, Vec56634 *dst, s32 val, s32 val2);
extern void func_80017B60(s32 id);
extern void func_800177AC(Vec56634 *position);
extern void func_80017860(s32 yaw, s32 pitch, s32 roll);

void func_80056634(void)
{
    s32 i;
    s32 angle;
    Entry56634 *entry;
    Vec56634 out;

    register Entry56634 *entries __asm__("$2");
    s32 index;

    index = D_8009F9A0->index;
    __asm__("" : : "r"(index));
    angle = D_8009F9A0->angle;
    entries = D_8009ACF4;
    entry = &entries[index];

    for (i = 0; i < 2; i++) {
        if (D_8009FA58[i] == 0) {
            continue;
        }

        func_8005612C(&D_8009F9A0->pos, angle, angle + entry->f0, entry->f2,
                      entry->f4, &entry->pos, &out,
                      D_8009FA58[i]->info->valA8, D_8009FA58[i]->info->valBC);

        func_80017B60(D_8009FA58[i]->info->idAC);
        func_800177AC(&out);
        func_80017860(angle, entry->f2, 0);

        {
            Info56634 *info = D_8009FA58[i]->info;
            info->flagA7 = 1;
            info->flagA6 = 1;
        }
        D_8009FA58[i]->info->valAA = 0;
    }
}
```


## Answer format

Deliver one markdown file with a ```json metadata block (`schema: fcq/answer-v1`, target,
run_id, model, status `exact|partial`, `source_sha256`, words, bytes, compiler,
`compiler_flags_extra`, `maspsx_flags_extra`, `link`, `gate_status`, `note`) and exactly one
```c block with the complete source that was measured. Then publish it through the run's
`fcq publish` command.
