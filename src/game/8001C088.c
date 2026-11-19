// Categories: Graphics > Fade Layers | Graphics > Display Setup
#include "game/types.h"

struct Unk1C040 {
    u8 unk00[0x18];
    u8 enabled;
    u8 red;
    u8 green;
    u8 blue;
    u8 unk1C[0x40];
};

extern s8 gSelectedLayer __asm__("D_8009F224");
extern u8 gLayerIndex __asm__("D_8009F225");
extern struct Unk1C040 gLayerBlocks[] __asm__("D_800A06E0");

extern void func_800932F0(struct Unk1C040 *entry);
void gSelectFadeLayer(u8 mode, u8 enable) __asm__("func_8001C088");


/**
 * @brief Selects a fade layer and enables or disables all of them.
 *
 * @param mode Layer index, stored in the signed global D_8009F224. Must be in
 *             0..D_8009F225-1 (the caller's responsibility; an out-of-range
 *             index selects past the array).
 * @param enable 1 = mark every layer enabled, 0 = mark every layer disabled.
 *
 * @return Nothing.
 *
 * @par Confirmed flow
 * D_8009F224 = mode; then the @c enabled byte at offset 0x18 of every
 * D_800A06E0[i] is set to 1 or 0, and func_800932F0(&D_800A06E0[mode]) uploads
 * the selected entry to the hardware.
 *
 * @par Globals
 * D_8009F224 (selection), D_8009F225 (layer count), D_800A06E0 (layers).
 *
 * @par Callers
 * func_80076EB0 (boot: select layer 1 and enable, then layer 0 and enable),
 * func_8001BFA8, gCloseVideoLayer, the unit at 0x80088124.
 *
 * @par Role and path
 * Selects fade layer mode in gSelectedLayer (D_8009F224), sets the enabled
 * byte on all gLayerBlocks (D_800A06E0) entries up to gLayerIndex
 * (D_8009F225) to 1 or 0, and calls func_800932F0 with the selected entry.
 * Called by func_80076EB0, func_8001BFA8, and func_80088120.
 */
void gSelectFadeLayer(u8 mode, u8 enable)
{
    s32 i;

    gSelectedLayer = mode;
    if (enable) {
        for (i = 0; i < gLayerIndex; i++) {
            gLayerBlocks[i].enabled = 1;
        }
    } else {
        for (i = 0; i < gLayerIndex; i++) {
            gLayerBlocks[i].enabled = 0;
        }
    }
    func_800932F0(&gLayerBlocks[gSelectedLayer]);
}
