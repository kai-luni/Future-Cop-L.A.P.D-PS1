// Categories: Utilities > Formatting | Platform > Library
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
/**
 * @brief First 12 bytes of the varargs formatter (sprintf): the register
 *        argument spill.
 *
 * The retail build stores $a1/$a2/$a3 into the caller's 16-byte argument save
 * area *before* the callee lowers $sp, so these three stores sit above the
 * frame set-up of the body at 0x8008CD3C. The catalogue cuts the unit right
 * after the third store, and a unit without prologue or epilogue cannot be
 * produced by a C function body (GCC always ends a fall-through block with
 * jr $ra/nop), so the words are carried as raw assembly.
 *
 * @param (implicit) $a0 destination buffer, $a1 format string, $a2/$a3 the
 *        first two variadic arguments - the usual o32 call layout. The body
 *        that follows at 0x8008CD3C consumes them.
 * @return Not applicable here; the body returns the number of characters
 *         written.
 *
 * @par Usage in the game
 * Every formatted path/name in the front-end goes through this entry, e.g.
 * sprintf(buf, "%s%s", prefix, name) - see func_80024E3C, func_80025370,
 * func_80083130 and the mission loaders.
 *
 * @par Role and path
 * Handwritten MIPS assembly spill prologue storing $a1, $a2, and $a3 into the
 * caller's o32 argument save area before falling through into the sprintf
 * formatter body at func_8008CD3C. Called by front-end and mission-loading
 * path formatters such as func_80024E3C, func_80025370, and func_80083130.
 */
__asm__(
    ".set noreorder\n"
    " .globl func_8008CD30\n"
    "func_8008CD30:\n"
    " sw $a1,4($sp)\n"
    " sw $a2,8($sp)\n"
    " sw $a3,12($sp)\n"
    ".set reorder\n");
