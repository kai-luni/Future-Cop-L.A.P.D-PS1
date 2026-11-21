// Categories: Graphics > Rendering | Resources > Memory Management
#include "game/types.h"

extern void *gCellMap __asm__("D_8009F820");
extern void *gCellEntityList[2] __asm__("D_8009F818");
extern void *D_8009F830;
extern void gFreeBlock(void *block) __asm__("func_800181D4");
void gFreeCellMapAndWorkBuffers(void) __asm__("func_80061860");


/**
 * @brief Frees the spatial visibility grid D_8009F820 and the render work buffers D_8009F818 and D_8009F830.
 *
 * Confirmed: passes D_8009F820 (16-byte-per-cell visibility grid allocated in func_80063DC8) to func_800181D4 when non-null, then unconditionally frees both 0x400-byte buffers in D_8009F818[0..1] and D_8009F830 (allocated in func_800617D8) without zeroing the pointers.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F820 - spatial visibility grid pointer (freed if non-null); D_8009F818 - 2-element array of 0x400-byte work buffers (freed); D_8009F830 - 0x400-byte work buffer (freed).
 *
 * @par Control flow
 * Non-null check on D_8009F820, two-iteration loop i = 0..1 freeing D_8009F818[i], and unconditional free of D_8009F830.
 *
 * @par Calls
 * func_800181D4 - frees D_8009F820, D_8009F818[0..1], and D_8009F830.
 *
 * @par Side effects
 * Releases up to four heap allocations via func_800181D4; leaves all global pointers intact.
 *
 * @par Callers
 * Confirmed: func_80024D14 (0x80024DB0 in asm/us/main.s, global subsystem reset) / Likely: none / Unknown: none.
 */
void gFreeCellMapAndWorkBuffers(void)
{
    s32 i;

    if (gCellMap != 0) {
        gFreeBlock(gCellMap);
    }
    for (i = 0; i < 2; i++) {
        gFreeBlock(gCellEntityList[i]);
    }
    gFreeBlock(D_8009F830);
}
