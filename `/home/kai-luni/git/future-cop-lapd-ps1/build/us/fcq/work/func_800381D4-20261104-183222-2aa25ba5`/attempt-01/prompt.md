# Decompilation brief — `func_800381D4` (`0x800381D4`, 224 bytes)

Repository: `/home/kai-luni/git/future-cop-lapd-ps1` — you can MEASURE there, so measure instead of guessing.
This target has **no candidate yet**: the function is open, nothing has been reconstructed.

## Acceptance rules

- A unit counts only when the project gate rebuilds **exactly** the retail bytes:
  `offline_decomp preview` prints `OK func_800381D4: <n> bytes`. A word-perfect count with a byte
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
nonmatching func_800381D4, 0xE0

glabel func_800381D4
    /* 289D4 800381D4 E8FFBD27 */  addiu      $sp, $sp, -0x18
    /* 289D8 800381D8 1000B0AF */  sw         $s0, 0x10($sp)
    /* 289DC 800381DC 21808000 */  addu       $s0, $a0, $zero
    /* 289E0 800381E0 1400BFAF */  sw         $ra, 0x14($sp)
    /* 289E4 800381E4 1801028E */  lw         $v0, 0x118($s0)
    /* 289E8 800381E8 00000000 */  nop
    /* 289EC 800381EC 821F0200 */  srl        $v1, $v0, 30
    /* 289F0 800381F0 05006010 */  beqz       $v1, .L80038208
    /* 289F4 800381F4 01000224 */   addiu     $v0, $zero, 0x1
    /* 289F8 800381F8 28006210 */  beq        $v1, $v0, .L8003829C
    /* 289FC 800381FC 00000000 */   nop
    /* 28A00 80038200 A8E00008 */  j          .L800382A0
    /* 28A04 80038204 00000000 */   nop
  .L80038208:
    /* 28A08 80038208 9A65000C */  jal        func_80019668
    /* 28A0C 8003820C 00000000 */   nop
    /* 28A10 80038210 0F004230 */  andi       $v0, $v0, 0xF
    /* 28A14 80038214 0C00422C */  sltiu      $v0, $v0, 0xC
    /* 28A18 80038218 21004010 */  beqz       $v0, .L800382A0
    /* 28A1C 8003821C 0040033C */   lui       $v1, (0x40000000 >> 16)
    /* 28A20 80038220 1801028E */  lw         $v0, 0x118($s0)
    /* 28A24 80038224 36010496 */  lhu        $a0, 0x136($s0)
    /* 28A28 80038228 25104300 */  or         $v0, $v0, $v1
    /* 28A2C 8003822C 180102AE */  sw         $v0, 0x118($s0)
    /* 28A30 80038230 A4000296 */  lhu        $v0, 0xA4($s0)
    /* 28A34 80038234 30010386 */  lh         $v1, 0x130($s0)
    /* 28A38 80038238 AC0000AE */  sw         $zero, 0xAC($s0)
    /* 28A3C 8003823C B40004A6 */  sh         $a0, 0xB4($s0)
    /* 28A40 80038240 00014234 */  ori        $v0, $v0, 0x100
    /* 28A44 80038244 A40002A6 */  sh         $v0, 0xA4($s0)
    /* 28A48 80038248 9A65000C */  jal        func_80019668
    /* 28A4C 8003824C B00003AE */   sw        $v1, 0xB0($s0)
    /* 28A50 80038250 03004230 */  andi       $v0, $v0, 0x3
    /* 28A54 80038254 36010486 */  lh         $a0, 0x136($s0)
    /* 28A58 80038258 00000000 */  nop
    /* 28A5C 8003825C 06008010 */  beqz       $a0, .L80038278
    /* 28A60 80038260 01004324 */   addiu     $v1, $v0, 0x1
    /* 28A64 80038264 01088228 */  slti       $v0, $a0, 0x801
    /* 28A68 80038268 07004014 */  bnez       $v0, .L80038288
    /* 28A6C 8003826C 00000000 */   nop
  .L80038270:
    /* 28A70 80038270 A2E00008 */  j          .L80038288
    /* 28A74 80038274 23180300 */   negu      $v1, $v1
  .L80038278:
    /* 28A78 80038278 01006230 */  andi       $v0, $v1, 0x1
    /* 28A7C 8003827C FCFF4014 */  bnez       $v0, .L80038270
    /* 28A80 80038280 00000000 */   nop
    /* 28A84 80038284 AA0003A6 */  sh         $v1, 0xAA($s0)
  .L80038288:
    /* 28A88 80038288 1801028E */  lw         $v0, 0x118($s0)
    /* 28A8C 8003828C AA0003A6 */  sh         $v1, 0xAA($s0)
    /* 28A90 80038290 FAFF0324 */  addiu      $v1, $zero, -0x6
    /* 28A94 80038294 24104300 */  and        $v0, $v0, $v1
    /* 28A98 80038298 180102AE */  sw         $v0, 0x118($s0)
  .L8003829C:
    /* 28A9C 8003829C 1C0000A6 */  sh         $zero, 0x1C($s0)
  .L800382A0:
    /* 28AA0 800382A0 1400BF8F */  lw         $ra, 0x14($sp)
    /* 28AA4 800382A4 1000B08F */  lw         $s0, 0x10($sp)
    /* 28AA8 800382A8 1800BD27 */  addiu      $sp, $sp, 0x18
    /* 28AAC 800382AC 0800E003 */  jr         $ra
    /* 28AB0 800382B0 00000000 */   nop
endlabel func_800381D4
```

### Neighbours of this function (1 verified, 1 open)

#### Callers

Open — no C exists for these; read the argument shape from the oracle around each `jal` (which registers are loaded right before it), never invent it:

| function | address | size |
|---|---|---|
| `func_800382B4` | `0x800382B4` | 1184 B |

#### Callees

| function | address | size |
|---|---|---|
| `func_80019668` | `0x80019668` | 92 B |

The sources of the verified neighbours that fit the size budget are printed in the *Verified neighbours* section below; the rest are described by their call sites in the oracle.

### Verified neighbours

**Verified callee `func_80019668`** (`0x80019668`, `0x5C` bytes, compiler `gcc-2.7.2-cdk`)

```c
#include "game/types.h"

extern s32 D_8009F5A8;
extern s32 D_800A0480[];

/* additive Lagged-Fibonacci-Generator, 31 bit random number */
s32 func_80019668(void)
{
    s32 i;
    s32 j;

    i = D_8009F5A8 - 1;
    j = D_8009F5A8 + 2;
    if (i < 0) {
        i = 0x13;
    } else if (j >= 0x14) {
        j = D_8009F5A8 - 0x12;
    }
    D_8009F5A8 = i;
    return D_800A0480[i] += D_800A0480[j];
}
```


### Shape-similar verified units

`func_8003FA34` (`0xA0` bytes):

```c
#include "game/types.h"

struct Unk3FA34Link {
    struct Unk3FA34Link *next;
    void *object;
};

extern void *D_8009F70C;
extern struct Unk3FA34Link *D_8009F708;
extern s32 D_8009FA00[];

extern void func_8003BC0C(void *object);
extern u8 func_8003B838(void *object);
extern struct Unk3FA34Link *func_800195A4(void *pool);

void func_8003FA34(void *object)
{
    struct Unk3FA34Link *link;

    if (D_8009F70C == 0 || D_8009FA00[0] == 4 || D_8009FA00[0] == 0xD) {
        func_8003BC0C(object);
        return;
    }
    if (func_8003B838(object) != 0) {
        link = func_800195A4(D_8009F70C);
        if (link != 0) {
            link->object = object;
            if (D_8009F708 == 0) {
                link->next = 0;
            } else {
                link->next = D_8009F708;
            }
            D_8009F708 = link;
        }
    }
}
```

`func_8003526C` (`0xC0` bytes):

```c
#include "game/types.h"

struct State {
    u8 pad00[0x18];
    s16 unk18;
    u8 pad1A[4];
    s16 unk1E;
    u16 unk20;
    u8 pad22[2];
    s32 unk24;
};

struct Entity {
    u8 pad00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 unk20;
    u8 pad21[3];
    u32 unk24;
    u8 pad28[0xCC];
    s32 unkF4;
    s16 unkF8;
    u8 unkFA;
    u8 padFB[0x39];
    struct State *unk134;
};

struct GlobalFA94 {
    u8 pad00[0xE0];
    s32 unkE0;
};

extern struct GlobalFA94 *D_8009FA94;

extern void func_8003BA50(struct Entity *self, s32 old1C);
extern s32 func_80019668(void);
extern void func_800330E8(struct Entity *self);

void func_8003526C(struct Entity *self)
{
    s32 old1C = self->unk1C;
    self->unk1C = self->unk1E;
    if (self->unk20 != 0 && !(self->unk24 & 0x40000)) {
        func_8003BA50(self, old1C);
    }
    self->unk134->unk1E = ((s16)self->unk1E >> 1) - (func_80019668() & 0x3F);
    self->unkFA = 0;
    func_800330E8(self);
    self->unk134->unk18 = 0;
    self->unkF4 = self->unk134->unk24;
    self->unkF8 = self->unk134->unk20;
    D_8009FA94->unkE0 = 4;
}
```

`func_80071A68` (`0xD8` bytes):

```c
#include "game/types.h"

struct Unk71A68Entry {
    void *data;
    void *extra;
    s32 unk08;
    s32 unk0C;
};

struct Unk71A68 {
    s32 unk00;
    void *first;
    void *second;
    s16 count;
    s16 unk0E;
    struct Unk71A68Entry *entries;
};

extern void func_80070710(void *block);
extern void func_800181D4(void *block);

void func_80071A68(struct Unk71A68 *object)
{
    s32 i;

    if (object->first != 0) {
        func_80070710(object->first);
    }
    if (object->second != 0) {
        func_80070710(object->second);
    }
    for (i = 0; i < object->count; i++) {
        if (object->entries[i].extra != 0) {
            func_800181D4(object->entries[i].extra);
        }
        func_800181D4(object->entries[i].data);
    }
    func_800181D4(object->entries);
    func_800181D4(object);
}
```


## Answer format

Deliver one markdown file with a ```json metadata block (`schema: fcq/answer-v1`, target,
run_id, model, status `exact|partial`, `source_sha256`, words, bytes, compiler,
`compiler_flags_extra`, `maspsx_flags_extra`, `link`, `gate_status`, `note`) and exactly one
```c block with the complete source that was measured. Then publish it through the run's
`fcq publish` command.
