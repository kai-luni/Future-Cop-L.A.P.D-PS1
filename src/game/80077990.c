/*
 * func_80077990 — 12-byte leaf fragment at 0x80077990.
 *
 * The analyser cut this unit out of the 0x144-byte retail block that starts
 * here: the three instructions below are the hoisted global loads that precede
 * the real function body at 0x8007799C (the next catalogue row).  There is no
 * return in the 12 bytes the unit owns, so it is written as an assembler
 * fragment; the same shape is used by src/game/80078C88.c and
 * src/game/800867B0.c.
 *
 *   lui   $v0, %hi(D_8009FA2C)     ; base of the item table
 *   lw    $v0, %lo(D_8009FA2C)($v0)
 *   lbu   $v1, 0x1D0($gp)          ; D_8009F3E4, the "already set up" flag
 *
 * D_8009F8E8 belongs to the rest of the container block (sw $v0,0x6D4($gp) at
 * 0x800779B0); the linker has to define it for this unit as well.
 */

__asm__(
".globl D_8009F3E4\n"
".globl D_8009FA2C\n"
".globl D_8009F8E8\n"
".text\n"
".set noreorder\n"
".globl func_80077990\n"
".ent func_80077990\n"
"func_80077990:\n"
" lui $2, %hi(D_8009FA2C)\n"
" lw $2, %lo(D_8009FA2C)($2)\n"
" lbu $3, 0x1d0($28)\n"
".end func_80077990\n"
".set reorder\n"
);
