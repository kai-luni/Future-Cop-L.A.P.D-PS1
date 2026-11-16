// Categories: Graphics > Scene Graph | Resources > Memory Pools | Gameplay > Map
#include "game/types.h"

extern s32 *D_8009F938;
extern s32 D_8009F93C;
extern s32 D_8009F940;

extern void *func_80017EBC(s32 size, s32 tag);

/**
 * @brief Allocates and zeroes the 8-byte-per-cell scene-graph bucket grid D_8009F938.
 *
 * Confirmed: stores width and height in D_8009F93C and D_8009F940, allocates width * height * 8 bytes via func_80017EBC with mode 2 into D_8009F938, and clears both words of every cell downward. Paired with GRDB map init func_80063DC8 and teardown func_80083428.
 *
 * @param width Grid width in cells stored in D_8009F93C.
 * @param height Grid height in cells stored in D_8009F940.
 *
 * @par Globals
 * D_8009F93C - bucket grid width (written); D_8009F940 - bucket grid height (written); D_8009F938 - allocated two-word cell array base (written).
 *
 * @par Control flow
 * Computes count = width * height, allocates count * 8 bytes, and zeroes pairs cells[i * 2] and cells[i * 2 + 1] from count - 1 down to 0.
 *
 * @par Calls
 * func_80017EBC - allocates count * 8 bytes under allocation mode/tag 2.
 *
 * @par Side effects
 * Allocates heap memory (mode 2), writes D_8009F93C, D_8009F940, D_8009F938, and zeroes all bucket entries.
 *
 * @par Callers
 * Confirmed: func_80063DC8 (0x80063FE0 in asm/us/main.s, GRDB map init) / Likely: none / Unknown: none.
 */
void func_800834C4(s32 width, s32 height)
{
    s32 *cells;
    s32 count;
    s32 i;

    count = width * height;
    D_8009F940 = height;
    D_8009F93C = width;
    cells = func_80017EBC(count * 8, 2);
    D_8009F938 = cells;
    for (i = count - 1; i >= 0; i--) {
        cells[i * 2] = 0;
        cells[i * 2 + 1] = 0;
    }
}
