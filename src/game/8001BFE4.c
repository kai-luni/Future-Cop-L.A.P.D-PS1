// Categories: Graphics > Fade Layers | Platform > GPU
#include "game/types.h"

struct Unk1BFE4 {
    u8 data[0x14];
};

extern s8 gSelectedLayer __asm__("D_8009F224");
extern u8 D_8009F226;
extern struct Unk1BFE4 gDrawBlocks[] __asm__("D_800A06B0");

extern void func_800934BC(struct Unk1BFE4 *entry);

/**
 * @brief Applies the currently selected layer/tile entry.
 *
 * @return 0 when D_8009F224 (the selected layer index, a signed byte) is >= 0
 *         and the entry was applied; 0xF when the index is negative - i.e. "no
 *         layer selected".
 *
 * @par Confirmed flow
 * D_8009F226 = D_8009F224 (the selection is copied to the "active" byte) and
 * func_800934BC(&D_800A06B0[D_8009F224]) uploads the 0x14-byte entry at that
 * index.
 *
 * @par Globals
 * D_8009F224 (selection), D_8009F226 (active copy), D_800A06B0 (0x14-byte
 * entry array).
 *
 * @par Callers
 * func_80076EB0 (boot), gCloseVideoLayer (image layer teardown), the unit at
 * 0x80088124 and func_8001EFF8 (the per-frame callback table).
 */
s32 func_8001BFE4(void)
{
    if (gSelectedLayer >= 0) {
        D_8009F226 = gSelectedLayer;
        func_800934BC(&gDrawBlocks[gSelectedLayer]);
        return 0;
    }
    return 0xF;
}
