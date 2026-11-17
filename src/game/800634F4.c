// Categories: Graphics > Terrain | Resources > Memory Management
#include "game/types.h"

extern void *gTileBlockTable __asm__("D_8009F808");
extern void *D_8009F810;
extern void *D_8009FA08;
extern void *D_800A294C[];
extern void func_800181D4(void *block);

/**
 * @brief Frees non-null map and mesh buffers D_8009F808, D_8009F810, D_8009FA08, and D_800A294C[0].
 *
 * Confirmed: checks each of the four globals for non-null and frees it via func_800181D4 without clearing the pointers. Counterpart to func_80063428 (allocates 0x1A0-byte D_8009F808 and 0x1B18-byte D_8009FA08) and func_80063DC8 (assigns D_8009F810).
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F808 - tile pointer buffer (freed); D_8009F810 - active map resource descriptor (freed); D_8009FA08 - 0x1B18-byte mesh work buffer (freed); D_800A294C[0] - terrain/map buffer (freed).
 *
 * @par Control flow
 * Four sequential non-null checks each gating one func_800181D4 call; no loops.
 *
 * @par Calls
 * func_800181D4 - frees each non-null buffer.
 *
 * @par Side effects
 * Releases up to four heap allocations via func_800181D4 without zeroing the global pointers.
 *
 * @par Callers
 * Confirmed: func_80024D14 (0x80024DA8 in asm/us/main.s, global subsystem reset) / Likely: none / Unknown: none.
 */
void func_800634F4(void)
{
    if (gTileBlockTable != 0) {
        func_800181D4(gTileBlockTable);
    }
    if (D_8009F810 != 0) {
        func_800181D4(D_8009F810);
    }
    if (D_8009FA08 != 0) {
        func_800181D4(D_8009FA08);
    }
    if (D_800A294C[0] != 0) {
        func_800181D4(D_800A294C[0]);
    }
}
