// Categories: Graphics > Effects
#include "game/types.h"

struct Unk4CC68 {
    u8 unk00[0x44];
    s32 unk44;
    s32 unk48;
    s32 unk4C;
};

extern s32 gFrameTime __asm__("D_8009FAA4");
extern struct Unk4CC68 gMarkerColorTable __asm__("D_800A6F60");
void gUpdateMarkerPulseColors(void) __asm__("func_8004CC68");


/**
 *  @brief Computes three packed values from a global phase-like counter.
 *
 *  Folds six bits of the shifted counter into a triangular level, adds an
 *  offset, clamps it, and packs the result into three state fields.
 *  @par Globals
 *  D_8009FAA4 supplies the counter; D_800A6F60 receives the packed values.
 *  @par Control flow
 *  Reflects levels at or above 32 and clamps the offset level to 31.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites D_800A6F60 fields unk44, unk48, and unk4C.
 *  @par Callers
 *  Confirmed: func_8002703C.
 */
void gUpdateMarkerPulseColors(void)
{
    s32 level;

    level = gFrameTime >> 3;
    level &= 0x3F;
    if (level >= 0x20) {
        level = 0x3F - level;
    }
    level += 8;
    if (level >= 0x20) {
        level = 0x1F;
    }
    gMarkerColorTable.unk44 = level << 3;
    gMarkerColorTable.unk48 = ((level / 2) << 3) | ((level / 2) << 11) | (level << 19);
    gMarkerColorTable.unk4C = level << 11;
}
