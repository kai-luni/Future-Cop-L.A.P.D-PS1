// Categories: Graphics > Lighting | Resources > Memory Pools | Gameplay > Map
#include "game/types.h"

extern void **gLightGrid __asm__("D_8009F780");
extern s32 gLightGridWidth __asm__("D_8009F78C");
extern s32 gLightGridHeight __asm__("D_8009F790");

extern void *func_80017EBC(s32 size, s32 tag);

/**
 * @brief Allocates and zeroes the spatial light-node bucket grid D_8009F780.
 *
 * Confirmed: stores width and height in D_8009F78C and D_8009F790, allocates width * height * 4 bytes via func_80017EBC with allocation mode 2 into D_8009F780, and clears all pointer slots downward. Paired with map header parser func_80063DC8 and teardown func_8004963C.
 *
 * @param width Number of grid columns stored in D_8009F78C.
 * @param height Number of grid rows stored in D_8009F790.
 *
 * @par Globals
 * D_8009F78C - grid column count (written); D_8009F790 - grid row count (written); D_8009F780 - allocated bucket pointer array base (written).
 *
 * @par Control flow
 * Computes count = width * height, allocates count * 4 bytes, and runs a reverse loop i = count - 1 down to 0 zeroing each slot.
 *
 * @par Calls
 * func_80017EBC - allocates count * 4 bytes under allocation tag/mode 2.
 *
 * @par Side effects
 * Allocates heap memory (mode 2), writes D_8009F78C, D_8009F790, D_8009F780, and zeroes the grid array.
 *
 * @par Callers
 * Confirmed: func_80063DC8 (0x80063FC4 in asm/us/main.s, GRDB map init) / Likely: none / Unknown: none.
 */
void func_800496D0(s32 width, s32 height)
{
    void **cells;
    s32 count;
    s32 i;

    count = width * height;
    gLightGridHeight = height;
    gLightGridWidth = width;
    cells = func_80017EBC(count * 4, 2);
    gLightGrid = cells;
    for (i = count - 1; i >= 0; i--) {
        cells[i] = 0;
    }
}
