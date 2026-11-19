// Categories: Graphics > Fade Layers | Platform > GPU
#include "game/types.h"

struct Unk1BFE4 {
    u8 data[0x14];
};

extern s8 gSelectedLayer __asm__("D_8009F224");
extern u8 gAppliedDisplayLayer __asm__("D_8009F226");
extern struct Unk1BFE4 gDrawBlocks[] __asm__("D_800A06B0");

extern void gPutDispEnvCached(struct Unk1BFE4 *entry) __asm__("func_800934BC");

s32 gApplySelectedDisplayEnv(void) __asm__("func_8001BFE4");

/**
 * @brief Installs the display environment selected by a signed layer index.
 * @return Zero after applying the selected entry, or 0x0F for a negative
 *     selection.
 *
 * @par Confirmed mechanics
 * Returns 0x0F on a negative selection. Otherwise copies the selection to
 * gAppliedDisplayLayer, scales it by 0x14, and passes the indexed environment
 * to gPutDispEnvCached.
 *
 * @par Globals
 * Reads gSelectedLayer and gDrawBlocks. Writes gAppliedDisplayLayer.
 *
 * @par Callers
 * Direct calls in the retail listing are from gPollPadButtons, gInitRuntime,
 * gCloseVideoLayer, gPlayVideoFile.
 *
 * @par Role and path
 * Boot and display mode transitions reuse this path to apply the selected
 * buffer's display settings.
 *
 * @par Unknown
 * The routine has no upper-bound check on a nonnegative index.
 */
s32 gApplySelectedDisplayEnv(void)
{
    if (gSelectedLayer >= 0) {
        gAppliedDisplayLayer = gSelectedLayer;
        gPutDispEnvCached(&gDrawBlocks[gSelectedLayer]);
        return 0;
    }
    return 0xF;
}
