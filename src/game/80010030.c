// Categories: Graphics > Display Setup | Platform > GTE
#include "game/types.h"

/*
 * func_80010030 sits in the hand-written GTE module at the start of .text
 * (the disassembler flags it "Handwritten function").  It is the only routine
 * in the executable that saves $ra alone at frame-4 (0x3C of a 0x40 frame);
 * every GCC 2.6.3-2.8.1 build of the equivalent C places a lone $ra at
 * frame-8 (0x38), so the body is kept as assembly.  Equivalent C:
 *
 *     void func_80010030(s32 x, s32 y, s32 h)
 *     {
 *         func_800959F0(x, y);
 *         func_80095A10(h);
 *     }
 */
extern void func_800959F0(s32 arg0, s32 arg1);
extern void func_80095A10(s32 arg0);

/**
 * @brief Two-stage display set-up: position (x, y) followed by a height value.
 *
 * @param x Forwarded unchanged to func_800959F0(x, y).
 * @param y Forwarded unchanged to func_800959F0(x, y).
 * @param h Forwarded to func_80095A10(h) after the position call.
 *
 * @return Nothing.
 *
 * @par Why this is assembly
 * The function lives in the hand-written GTE module at the start of .text. It
 * is the only routine in the executable that saves $ra alone at frame-4
 * (0x3C of a 0x40 frame); every GCC 2.6.3-2.8.1 build of the equivalent C puts
 * a lone $ra at frame-8 (0x38), so no compiler reproduces it. The equivalent C
 * is kept in the comment above the block:
 * @code
 * void func_80010030(s32 x, s32 y, s32 h) { func_800959F0(x, y); func_80095A10(h); }
 * @endcode
 *
 * @par Likely role
 * func_800959F0 takes the pair (x, y) and func_80095A10 a single height, which
 * matches "set the projection/viewport origin, then its size". Callers use
 * (0xA0, 0x78, 0x200) for the 320x240 front-end and (0x50, 0x78, 0x200) for the
 * narrower variant - consistent with centring a viewport.
 *
 * @par Callers
 * func_800287F4 (once per frame) and func_80019D60.
 */
__asm__(
    ".text\n\t"
    ".align 2\n\t"
    ".globl func_80010030\n\t"
    ".ent func_80010030\n"
    "func_80010030:\n\t"
    ".set\tnoreorder\n\t"
    ".set\tnomacro\n\t"
    "addiu $29,$29,-64\n\t"
    "sw $4,64($29)\n\t"
    "sw $5,68($29)\n\t"
    "lw $4,64($29)\n\t"
    "lw $5,68($29)\n\t"
    "sw $31,60($29)\n\t"
    ".word 0x0C02567C\n\t" /* jal func_800959F0 */
    "sw $6,72($29)\n\t"
    ".word 0x8FA40048\n\t" /* lw $4,72($29) */
    ".word 0x0C025684\n\t" /* jal func_80095A10 */
    "nop\n\t"
    "lw $31,60($29)\n\t"
    "nop\n\t"
    ".word 0x03E00008\n\t" /* jr $31 */
    "addiu $29,$29,64\n\t"
    ".set\tmacro\n\t"
    ".set\treorder\n\t"
    ".end func_80010030\n");
