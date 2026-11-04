#include "game/types.h"

/* 0x8008CD30 (0xC bytes) — argument-spill head of the varargs formatter whose
 * body the analysis attributes to the next unit at 0x8008CD3C.
 *
 * The retail build spills the register arguments $a1/$a2/$a3 into the o32
 * caller's 16-byte argument save area *before* the callee adjusts $sp, so the
 * three stores sit above the frame setup (addiu $sp,$sp,-592) at 0x8008CD3C.
 * The catalogue cuts this unit right after the third store, which means the
 * extent carries no prologue and no epilogue at all: nothing a C function body
 * can emit, because GCC always terminates a fall-through with jr $ra/nop.
 * The unit is therefore carried as the raw spill GCC placed here. */
__asm__(
    ".set noreorder\n"
    " .globl func_8008CD30\n"
    "func_8008CD30:\n"
    " sw $a1,4($sp)\n"
    " sw $a2,8($sp)\n"
    " sw $a3,12($sp)\n"
    ".set reorder\n");
