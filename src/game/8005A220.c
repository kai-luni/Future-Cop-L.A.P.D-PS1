// Categories: Graphics > Textures | Resources > Memory Management
#include "game/types.h"

extern void **gTextureDescriptorTable __asm__("D_8009F7D8");
extern s32 gTexDescriptorCount __asm__("D_8009F7DC");

extern void gFreeBlock(void *block) __asm__("func_800181D4");
void gFreeTextureDescriptors(void) __asm__("func_8005A220");


/**
 * @brief Frees all per-entry sprite/tile descriptor blocks in D_8009F7D8 and resets D_8009F7DC.
 *
 * Confirmed: when D_8009F7D8 is non-null, frees D_8009F7D8[i] in reverse order from D_8009F7DC - 1 down to 0 via func_800181D4, frees D_8009F7D8 itself, and nulls D_8009F7D8; unconditionally clears D_8009F7DC = 0. Companion teardown to func_8005A414.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F7D8 - pointer array of allocated sprite/tile descriptor blocks (freed and zeroed); D_8009F7DC - entry count (read for loop bound, then cleared).
 *
 * @par Control flow
 * Guards array free loop on D_8009F7D8 != 0, iterates i = D_8009F7DC - 1 down to 0, then unconditionally sets D_8009F7DC = 0.
 *
 * @par Calls
 * func_800181D4 - frees each D_8009F7D8[i] entry block and the D_8009F7D8 table.
 *
 * @par Side effects
 * Releases heap allocations in D_8009F7D8 and clears both D_8009F7D8 and D_8009F7DC to 0.
 *
 * @par Callers
 * Confirmed: func_8005A290 (0x8005A298 in asm/us/main.s), func_8005A414 (0x8005A610 in asm/us/main.s, error cleanup) / Likely: none / Unknown: none.
 *
 * @par Environment path
 * Frees each per-entry sprite/tile descriptor block in
 * gTextureDescriptorTable in reverse order via gFreeBlock, frees the table
 * itself, and resets D_8009F7DC to 0 (called by func_8005A290 and
 * func_8005A414).
 */
void gFreeTextureDescriptors(void)
{
    s32 i;

    if (gTextureDescriptorTable != 0) {
        for (i = gTexDescriptorCount - 1; i >= 0; i--) {
            gFreeBlock(gTextureDescriptorTable[i]);
        }
        gFreeBlock(gTextureDescriptorTable);
        gTextureDescriptorTable = 0;
    }
    gTexDescriptorCount = 0;
}
