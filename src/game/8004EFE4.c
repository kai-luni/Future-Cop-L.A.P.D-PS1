// Categories: Resources > Memory Management | Graphics > Renderer
#include "game/types.h"

extern void gFreeBlock(s32 value) __asm__("func_800181D4");
extern u32 gFreedPointer __asm__("D_8009F7B0");
void gReleaseCoordinatePairStorage(s32 arg) __asm__("func_8004EFE4");


/**
 * @brief Frees arg if non-zero and clears the global pointer D_8009F7B0.
 *
 * Checks arg != 0 before calling func_800181D4(arg), then unconditionally sets D_8009F7B0 = 0 (0x59C($gp), read as a Pair pointer by func_8004F09C). In asm/us/main.s, an unlabeled lw $a0, 0x59C($gp) at 0x8004EFE0 precedes glabel func_8004EFE4.
 *
 * @param arg Block pointer (in $a0) to free if non-zero.
 *
 * @par Globals
 * D_8009F7B0 - Pair-table pointer (0x59C($gp)), cleared to 0.
 *
 * @par Control flow
 * Guards func_800181D4(arg) on arg != 0, then stores 0 to D_8009F7B0.
 *
 * @par Calls
 * func_800181D4 - releases arg when non-zero.
 *
 * @par Side effects
 * Frees arg via func_800181D4 if non-zero and sets D_8009F7B0 = 0.
 *
 * @par Callers
 * Confirmed: none in asm/us/main.s or analysis/edges.json / Likely: unused teardown counterpart to func_8004EFD4 / Unknown: any indirect caller.
 */
void gReleaseCoordinatePairStorage(s32 arg)
{
    if (arg != 0) {
        gFreeBlock(arg);
    }
    gFreedPointer = 0;
}
