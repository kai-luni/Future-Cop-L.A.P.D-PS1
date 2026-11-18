# Decompilation brief — `func_8005A828` (`0x8005A828`, 268 bytes)

Repository: `/home/kai-luni/git/future-cop-lapd-ps1` — you can MEASURE there, so measure instead of guessing.
This target has **no candidate yet**: the function is open, nothing has been reconstructed.

## Acceptance rules

- A unit counts only when the project gate rebuilds **exactly** the retail bytes:
  `offline_decomp preview` prints `OK func_8005A828: <n> bytes`. A word-perfect count with a byte
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
nonmatching func_8005A828, 0x10C

glabel func_8005A828
    /* 4B028 8005A828 D8FFBD27 */  addiu      $sp, $sp, -0x28
    /* 4B02C 8005A82C 1C00B1AF */  sw         $s1, 0x1C($sp)
    /* 4B030 8005A830 21888000 */  addu       $s1, $a0, $zero
    /* 4B034 8005A834 1800B0AF */  sw         $s0, 0x18($sp)
    /* 4B038 8005A838 2180C000 */  addu       $s0, $a2, $zero
    /* 4B03C 8005A83C 2120A000 */  addu       $a0, $a1, $zero
    /* 4B040 8005A840 1000A527 */  addiu      $a1, $sp, 0x10
    /* 4B044 8005A844 2000BFAF */  sw         $ra, 0x20($sp)
    /* 4B048 8005A848 1B61000C */  jal        func_8001846C
    /* 4B04C 8005A84C 1400A627 */   addiu     $a2, $sp, 0x14
    /* 4B050 8005A850 0A80033C */  lui        $v1, %hi(D_8009FA90)
    /* 4B054 8005A854 90FA638C */  lw         $v1, %lo(D_8009FA90)($v1)
    /* 4B058 8005A858 0A80023C */  lui        $v0, %hi(D_8009F9C0)
    /* 4B05C 8005A85C C0F9428C */  lw         $v0, %lo(D_8009F9C0)($v0)
    /* 4B060 8005A860 00000000 */  nop
    /* 4B064 8005A864 2B106200 */  sltu       $v0, $v1, $v0
    /* 4B068 8005A868 03004010 */  beqz       $v0, .L8005A878
    /* 4B06C 8005A86C 1C006224 */   addiu     $v0, $v1, 0x1C
    /* 4B070 8005A870 0A80013C */  lui        $at, %hi(D_8009FA90)
    /* 4B074 8005A874 90FA22AC */  sw         $v0, %lo(D_8009FA90)($at)
  .L8005A878:
    /* 4B078 8005A878 21400000 */  addu       $t0, $zero, $zero
    /* 4B07C 8005A87C 2C00093C */  lui        $t1, (0x2C0000 >> 16)
    /* 4B080 8005A880 0A80023C */  lui        $v0, %hi(D_8009AFB4)
    /* 4B084 8005A884 B4AF4424 */  addiu      $a0, $v0, %lo(D_8009AFB4)
    /* 4B088 8005A888 08006524 */  addiu      $a1, $v1, 0x8
    /* 4B08C 8005A88C 0004023C */  lui        $v0, (0x4000000 >> 16)
    /* 4B090 8005A890 000062AC */  sw         $v0, 0x0($v1)
    /* 4B094 8005A894 0020023C */  lui        $v0, (0x20000000 >> 16)
    /* 4B098 8005A898 25100202 */  or         $v0, $s0, $v0
    /* 4B09C 8005A89C 040062AC */  sw         $v0, 0x4($v1)
    /* 4B0A0 8005A8A0 1400A78F */  lw         $a3, 0x14($sp)
    /* 4B0A4 8005A8A4 1000A68F */  lw         $a2, 0x10($sp)
  .L8005A8A8:
    /* 4B0A8 8005A8A8 00008284 */  lh         $v0, 0x0($a0)
    /* 4B0AC 8005A8AC 00000000 */  nop
    /* 4B0B0 8005A8B0 1800E200 */  mult       $a3, $v0
    /* 4B0B4 8005A8B4 12180000 */  mflo       $v1
    /* 4B0B8 8005A8B8 02008284 */  lh         $v0, 0x2($a0)
    /* 4B0BC 8005A8BC 00000000 */  nop
    /* 4B0C0 8005A8C0 1800C200 */  mult       $a2, $v0
    /* 4B0C4 8005A8C4 12100000 */  mflo       $v0
    /* 4B0C8 8005A8C8 23106200 */  subu       $v0, $v1, $v0
    /* 4B0CC 8005A8CC 21102202 */  addu       $v0, $s1, $v0
    /* 4B0D0 8005A8D0 03140200 */  sra        $v0, $v0, 16
    /* 4B0D4 8005A8D4 0000A2A4 */  sh         $v0, 0x0($a1)
    /* 4B0D8 8005A8D8 00008284 */  lh         $v0, 0x0($a0)
    /* 4B0DC 8005A8DC 00000000 */  nop
    /* 4B0E0 8005A8E0 1800C200 */  mult       $a2, $v0
    /* 4B0E4 8005A8E4 12180000 */  mflo       $v1
    /* 4B0E8 8005A8E8 02008284 */  lh         $v0, 0x2($a0)
    /* 4B0EC 8005A8EC 00000000 */  nop
    /* 4B0F0 8005A8F0 1800E200 */  mult       $a3, $v0
    /* 4B0F4 8005A8F4 01000825 */  addiu      $t0, $t0, 0x1
    /* 4B0F8 8005A8F8 04008424 */  addiu      $a0, $a0, 0x4
    /* 4B0FC 8005A8FC 12100000 */  mflo       $v0
    /* 4B100 8005A900 21106200 */  addu       $v0, $v1, $v0
    /* 4B104 8005A904 21104900 */  addu       $v0, $v0, $t1
    /* 4B108 8005A908 03140200 */  sra        $v0, $v0, 16
    /* 4B10C 8005A90C 0200A2A4 */  sh         $v0, 0x2($a1)
    /* 4B110 8005A910 03000229 */  slti       $v0, $t0, 0x3
    /* 4B114 8005A914 E4FF4014 */  bnez       $v0, .L8005A8A8
    /* 4B118 8005A918 0400A524 */   addiu     $a1, $a1, 0x4
    /* 4B11C 8005A91C 2000BF8F */  lw         $ra, 0x20($sp)
    /* 4B120 8005A920 1C00B18F */  lw         $s1, 0x1C($sp)
    /* 4B124 8005A924 1800B08F */  lw         $s0, 0x18($sp)
    /* 4B128 8005A928 2800BD27 */  addiu      $sp, $sp, 0x28
    /* 4B12C 8005A92C 0800E003 */  jr         $ra
    /* 4B130 8005A930 00000000 */   nop
endlabel func_8005A828
```

### Neighbours of this function (1 verified, 1 open)

#### Callers

Open — no C exists for these; read the argument shape from the oracle around each `jal` (which registers are loaded right before it), never invent it:

| function | address | size |
|---|---|---|
| `func_8005A9C4` | `0x8005A9C4` | 456 B |

#### Callees

| function | address | size |
|---|---|---|
| `func_8001846C` | `0x8001846C` | 200 B |

The sources of the verified neighbours that fit the size budget are printed in the *Verified neighbours* section below; the rest are described by their call sites in the oracle.

### Verified neighbours

**Verified callee `func_8001846C`** (`0x8001846C`, `0xC8` bytes, compiler `gcc-2.7.2-cdk`)

```c
#include "game/types.h"

extern u16 D_8009A234[];

void func_8001846C(u32 angle, s32 *sine, s32 *cosine)
{
    s32 index;
    s32 s;
    s32 c;
    s32 quadrant;

    angle &= 0xFFF;
    index = angle & 0x3FF;
    s = D_8009A234[index] << 1;
    c = D_8009A234[0x400 - index] << 1;
    quadrant = (s32)angle >> 10;
    switch (quadrant) {
    case 0:
        *sine = s;
        *cosine = c;
        break;
    case 1:
        *sine = c;
        *cosine = -s;
        break;
    case 2:
        *sine = -s;
        *cosine = -c;
        break;
    case 3:
        *sine = -c;
        *cosine = s;
        break;
    }
}
```


### Shape-similar verified units

`func_8005A740` (`0xE8` bytes):

```c
#include "game/types.h"

struct LineF2 {
    u32 tag;
    u32 code;
    s16 x0;
    s16 y0;
    s16 x1;
    s16 y1;
};

extern u8 *D_8009FA90;
extern u8 *D_8009F9C0;

extern void func_8001846C(s32 id, s32 *first, s32 *second);

void func_8005A740(s32 origin, s32 id)
{
    struct LineF2 *line;
    s32 first;
    s32 second;

    func_8001846C(id, &first, &second);
    line = (struct LineF2 *)D_8009FA90;
    if (D_8009FA90 < D_8009F9C0) {
        D_8009FA90 += 0x1C;
    }
    line->tag = 0x3000000;
    line->code = 0x40808080;
    line->x0 = (origin + second * 24) >> 16;
    line->y0 = (first * 24 + 0x2C0000) >> 16;
    line->x1 = (origin + second * 30) >> 16;
    line->y1 = (first * 30 + 0x2C0000) >> 16;
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


## Answer format

Deliver one markdown file with a ```json metadata block (`schema: fcq/answer-v1`, target,
run_id, model, status `exact|partial`, `source_sha256`, words, bytes, compiler,
`compiler_flags_extra`, `maspsx_flags_extra`, `link`, `gate_status`, `note`) and exactly one
```c block with the complete source that was measured. Then publish it through the run's
`fcq publish` command.
