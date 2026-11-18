# Decompilation brief — `func_800537D0` (`0x800537D0`, 248 bytes)

Repository: `/home/kai-luni/git/future-cop-lapd-ps1` — you can MEASURE there, so measure instead of guessing.
This target has **no candidate yet**: the function is open, nothing has been reconstructed.

## Acceptance rules

- A unit counts only when the project gate rebuilds **exactly** the retail bytes:
  `offline_decomp preview` prints `OK func_800537D0: <n> bytes`. A word-perfect count with a byte
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
nonmatching func_800537D0, 0xF8

glabel func_800537D0
    /* 43FD0 800537D0 E8FFBD27 */  addiu      $sp, $sp, -0x18
    /* 43FD4 800537D4 2800A88F */  lw         $t0, 0x28($sp)
    /* 43FD8 800537D8 3300A010 */  beqz       $a1, .L800538A8
    /* 43FDC 800537DC 1000BFAF */   sw        $ra, 0x10($sp)
    /* 43FE0 800537E0 A0008A8C */  lw         $t2, 0xA0($a0)
    /* 43FE4 800537E4 A8008B8C */  lw         $t3, 0xA8($a0)
    /* 43FE8 800537E8 1D00C014 */  bnez       $a2, .L80053860
    /* 43FEC 800537EC 2160A000 */   addu      $t4, $a1, $zero
    /* 43FF0 800537F0 1E000011 */  beqz       $t0, .L8005386C
    /* 43FF4 800537F4 40180500 */   sll       $v1, $a1, 1
    /* 43FF8 800537F8 21186500 */  addu       $v1, $v1, $a1
    /* 43FFC 800537FC 00110300 */  sll        $v0, $v1, 4
    /* 44000 80053800 23104300 */  subu       $v0, $v0, $v1
    /* 44004 80053804 80100200 */  sll        $v0, $v0, 2
    /* 44008 80053808 2128A200 */  addu       $a1, $a1, $v0
    /* 4400C 8005380C 0200A104 */  bgez       $a1, .L80053818
    /* 44010 80053810 2118A000 */   addu      $v1, $a1, $zero
    /* 44014 80053814 FF00A324 */  addiu      $v1, $a1, 0xFF
  .L80053818:
    /* 44018 80053818 032A0300 */  sra        $a1, $v1, 8
    /* 4401C 8005381C 2160A000 */  addu       $t4, $a1, $zero
    /* 44020 80053820 0100A328 */  slti       $v1, $a1, 0x1
    /* 44024 80053824 01000224 */  addiu      $v0, $zero, 0x1
    /* 44028 80053828 05000215 */  bne        $t0, $v0, .L80053840
    /* 4402C 8005382C 01006638 */   xori      $a2, $v1, 0x1
    /* 44030 80053830 05006014 */  bnez       $v1, .L80053848
    /* 44034 80053834 21104001 */   addu      $v0, $t2, $zero
    /* 44038 80053838 164E0108 */  j          .L80053858
    /* 4403C 8005383C 21504B01 */   addu      $t2, $t2, $t3
  .L80053840:
    /* 44040 80053840 0400C010 */  beqz       $a2, .L80053854
    /* 44044 80053844 21104001 */   addu      $v0, $t2, $zero
  .L80053848:
    /* 44048 80053848 23504B01 */  subu       $t2, $t2, $t3
    /* 4404C 8005384C 1B4E0108 */  j          .L8005386C
    /* 44050 80053850 21586201 */   addu      $t3, $t3, $v0
  .L80053854:
    /* 44054 80053854 21504B01 */  addu       $t2, $t2, $t3
  .L80053858:
    /* 44058 80053858 1B4E0108 */  j          .L8005386C
    /* 4405C 8005385C 23586201 */   subu      $t3, $t3, $v0
  .L80053860:
    /* 44060 80053860 21104001 */  addu       $v0, $t2, $zero
    /* 44064 80053864 21506001 */  addu       $t2, $t3, $zero
    /* 44068 80053868 23580200 */  negu       $t3, $v0
  .L8005386C:
    /* 4406C 8005386C 18008A01 */  mult       $t4, $t2
    /* 44070 80053870 12400000 */  mflo       $t0
    /* 44074 80053874 10480000 */  mfhi       $t1
    /* 44078 80053878 004C0900 */  sll        $t1, $t1, 16
    /* 4407C 8005387C 02440800 */  srl        $t0, $t0, 16
    /* 44080 80053880 25682801 */  or         $t5, $t1, $t0
    /* 44084 80053884 0000EDAC */  sw         $t5, 0x0($a3)
    /* 44088 80053888 1800AB00 */  mult       $a1, $t3
    /* 4408C 8005388C 12400000 */  mflo       $t0
    /* 44090 80053890 10480000 */  mfhi       $t1
    /* 44094 80053894 004C0900 */  sll        $t1, $t1, 16
    /* 44098 80053898 02440800 */  srl        $t0, $t0, 16
    /* 4409C 8005389C 25682801 */  or         $t5, $t1, $t0
    /* 440A0 800538A0 2C4E0108 */  j          .L800538B0
    /* 440A4 800538A4 0800EDAC */   sw        $t5, 0x8($a3)
  .L800538A8:
    /* 440A8 800538A8 0000E0AC */  sw         $zero, 0x0($a3)
    /* 440AC 800538AC 0800E0AC */  sw         $zero, 0x8($a3)
  .L800538B0:
    /* 440B0 800538B0 CC4D010C */  jal        func_80053730
    /* 440B4 800538B4 2128E000 */   addu      $a1, $a3, $zero
    /* 440B8 800538B8 1000BF8F */  lw         $ra, 0x10($sp)
    /* 440BC 800538BC 1800BD27 */  addiu      $sp, $sp, 0x18
    /* 440C0 800538C0 0800E003 */  jr         $ra
    /* 440C4 800538C4 00000000 */   nop
endlabel func_800537D0
```

### Neighbours of this function (1 verified, 1 open)

#### Callers

Open — no C exists for these; read the argument shape from the oracle around each `jal` (which registers are loaded right before it), never invent it:

| function | address | size |
|---|---|---|
| `func_80053A74` | `0x80053A74` | 2844 B |

#### Callees

| function | address | size |
|---|---|---|
| `func_80053730` | `0x80053730` | 160 B |

The sources of the verified neighbours that fit the size budget are printed in the *Verified neighbours* section below; the rest are described by their call sites in the oracle.

### Verified neighbours

**Verified callee `func_80053730`** (`0x80053730`, `0xA0` bytes, compiler `gcc-2.7.2-cdk`)

```c
#include "game/types.h"

struct Unk53730Info {
    u8 unk00[0xC1];
    s8 side;
};

struct Unk53730 {
    u8 unk00[0x50];
    s32 x;
    s32 y;
    s32 z;
    u8 unk5C[0x50];
    struct Unk53730Info *info;
};

struct Unk53730Global {
    u8 unk00[0x5C];
    s32 unk5C;
};

struct Unk53730Delta {
    s32 x;
    s32 z;
};

extern s32 D_8009FA0C[];
extern struct Unk53730Global *D_8009FA94[];
extern struct Unk53730 *D_8009FA58[];
extern s32 D_8009FA64;

extern s32 func_8001A074(struct Unk53730Delta *delta);

void func_80053730(struct Unk53730 *object)
{
    struct Unk53730 *other;
    struct Unk53730Delta delta;

    if (D_8009FA0C[0] == 2 && D_8009FA94[0]->unk5C == 0) {
        other = D_8009FA58[1 - object->info->side];
        delta.x = other->x - object->x;
        delta.z = other->z - object->z;
        D_8009FA64 = func_8001A074(&delta);
    }
}
```


### Shape-similar verified units

`func_80046F9C` (`0xEC` bytes):

```c
#include "game/types.h"

struct SoundBank {
    s32 samples[4];
    u8 count;
    u8 cursor;
    u8 voices;
    u8 unk13;
    u8 unk14;
    u8 unk15;
    u8 key;
    u8 unk17;
    s32 head;
};

extern volatile u8 D_8009F340;
extern struct SoundBank D_800A7938[];

extern s32 func_800458E8(s32 arg0, s32 arg1, s8 *mix, s32 arg3);
extern s32 func_80046B04(s32 volume, s32 arg1, s32 arg2, u32 index);
extern void func_800460D4(s8 *mix);
extern u8 func_8004772C(struct SoundBank *bank, s32 handle, s32 pan, s32 volume, s32 extra);
extern void func_80046698(s32 handle);

s32 func_80046F9C(s32 arg0, s32 arg1, u32 index, s32 arg3, s32 arg4)
{
    struct SoundBank *bank;
    register s32 handle __asm__("$17");
    register u32 indexValue __asm__("$18");
    register s32 arg4Value __asm__("$19");
    register u32 enabled __asm__("$2");
    s8 mix[0x18];

    enabled = D_8009F340;
    arg4Value = arg4;
    __asm__("" : "=r"(arg4Value) : "0"(arg4Value));
    indexValue = index;
    __asm__("" : "=r"(indexValue) : "0"(indexValue));
    handle = 0;
    __asm__("" : "=r"(handle) : "0"(handle));
    if (enabled == 0) {
        return handle;
    }
    if (indexValue >= 0x6E) {
        return handle;
    }
    bank = &D_800A7938[indexValue];
    if (bank->count == 0) {
        return handle;
    }
    if (func_800458E8(arg0, arg1, mix, arg3) == 0) {
        return handle;
    }
    handle = func_80046B04(mix[0], bank->unk13, (s8)arg4Value, indexValue);
    if (handle != 0) {
        func_800460D4(mix);
        if (func_8004772C(bank, handle, mix[1], mix[0], -1) == 0) {
            func_80046698(handle);
            handle = 0;
        }
    }
    return handle;
}
```

`func_8001A578` (`0xCC` bytes):

```c
#include "game/types.h"

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);

void func_8001A578(u32 angle, s32 *vec)
{
    s32 sine;
    s32 cosine;
    register s32 p0 __asm__("$3");
    register s32 b __asm__("$2");
    register s32 a __asm__("$4");
    register s32 x __asm__("$5");

    func_8001846C(angle, &sine, &cosine);

    x = vec[0];
    b = cosine;
    FMUL(p0, b, x);
    a = sine;
    b = vec[1];
    FMUL(b, a, b);
    p0 -= b;
    vec[0] = p0;

    b = sine;
    FMUL(p0, b, x);
    a = cosine;
    b = vec[1];
    FMUL(b, a, b);
    p0 += b;
    vec[1] = p0;
}
```

`func_8001A180` (`0x194` bytes):

```c

#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

extern s32 func_80095C14(u32 value);
extern s32 func_80019904(s32 value);

void func_8001A180(struct Vec3s32 *vector)
{
    s32 x;
    s32 y;
    s32 z;
    s32 ax;
    s32 ay;
    s32 az;
    s32 diff;
    register s32 shift __asm__("$4");
    s32 sum;
    s32 scale;
    s32 m0;
    register s32 p0 __asm__("$2");
    register s32 p1 __asm__("$3");

    x = vector->x;
    y = vector->y;
    z = vector->z;

    ax = x;
    ay = y;
    az = z;

    if (ax < 0) {
        ax = -ax;
    }
    if (ay < 0) {
        ay = -ay;
    }
    if (az < 0) {
        az = -az;
    }

    diff = ay - ax;
    ay -= (diff >> 31) & diff;

    diff = az - ay;
    ay = az - ((diff >> 31) & diff);

    if (ay != 0) {
        shift = func_80095C14((u32)ay);
        p1 = 0x1F;
        shift = p1 - shift;
    } else {
        shift = 0;
    }

    if (shift >= 0x15) {
        shift -= 0x14;
        x >>= shift;
        y >>= shift;
        z >>= shift;
    } else {
        shift = 0x14 - shift;
        x <<= shift;
        y <<= shift;
        z <<= shift;
    }

    FMUL(p0, x, x);
    FMUL(p1, y, y);
    FMUL(shift, z, z);
    sum = p0 + p1 + shift;

    if (sum > 0) {
        scale = func_80019904(sum);
        FMUL(m0, x, scale);
        vector->x = m0;
        FMUL(m0, y, scale);
        vector->y = m0;
        FMUL(m0, z, scale);
        vector->z = m0;
    }
}
```


## Answer format

Deliver one markdown file with a ```json metadata block (`schema: fcq/answer-v1`, target,
run_id, model, status `exact|partial`, `source_sha256`, words, bytes, compiler,
`compiler_flags_extra`, `maspsx_flags_extra`, `link`, `gate_status`, `note`) and exactly one
```c block with the complete source that was measured. Then publish it through the run's
`fcq publish` command.


## Previous attempt: 1/62 words, 39/248 bytes, emitted 240
Profile: {"compiler": "gcc-2.7.2-cdk", "compiler_flags_extra": ["-G4", "-O2"], "link": true, "maspsx_flags_extra": [], "rodata_address": null}

### Instructions that differ (retail vs ours)
```
0x800537D4  retail: 2800A88F  ours:   1000BFAF
0x800537D8  retail: 3300A010  ours:   A0008A8C
0x800537DC  retail: 1000BFAF  ours:   A800848C
0x800537E0  retail: A0008A8C  ours:   2800A88F
0x800537E4  retail: A8008B8C  ours:   0400A014
0x800537E8  retail: 1D00C014  ours:   2158A000
0x800537EC  retail: 2160A000  ours:   0000E0AC
0x800537F0  retail: 1E000011  ours:   2A4E0108
0x800537F4  retail: 40180500  ours:   0400E0AC
0x800537F8  retail: 21186500  ours:   0400C010
0x800537FC  retail: 00110300  ours:   21184001
0x80053800  retail: 23104300  ours:   21508000
```

### The C that produced this score
```c
#include "game/types.h"

struct Unk537D0 {
    u8 pad0[0xA0];
    s32 fieldA0;
    u8 pad1[0x4];
    s32 fieldA8;
};

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

extern void func_80053730(void *a0);

void func_800537D0(struct Unk537D0 *a0, s32 a1, s32 a2, s32 *a3, s32 a4)
{
    s32 t2 = a0->fieldA0;
    s32 t3 = a0->fieldA8;
    s32 t4 = a1;
    s32 tmp;

    if (a1 == 0) {
        a3[0] = 0;
        a3[1] = 0;
    } else {
        if (a2 != 0) {
            tmp = t2;
            t2 = t3;
            t3 = -tmp;
        } else if (a4 != 0) {
            a1 = (a1 * 181) / 256;
            t4 = a1;
            tmp = t2;
            if (a4 == 1) {
                if (a1 < 1) {
                    t2 -= t3;
                    t3 += tmp;
                } else {
                    t2 += t3;
                    t3 -= 1;
                }
            } else {
                if (a1 < 1) {
                    t2 -= t3;
                    t3 += tmp;
                } else {
                    t2 += t3;
                    t3 -= tmp;
                }
            }
        }
        FMUL(tmp, t4, t2);
        a3[0] = tmp;
        FMUL(tmp, a1, t3);
        a3[1] = tmp;
    }
    func_80053730(a3);
}
```
