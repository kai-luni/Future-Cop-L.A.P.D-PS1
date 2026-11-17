// Categories: Graphics > HUD > Minimap | Utilities > Mathematics
#include "game/types.h"

extern s32 gDiscShadeByte __asm__("D_8009FAA4");

/**
 * @brief Converts one world coordinate into a minimap cell index, with a
 *        special band for far values.
 *
 * @param arg0 Signed world coordinate (0 is the centre).
 * @return 0 for arg0 == 0; for arg0 >= 0x21 the parity bit of
 *         ((arg0 - 0x20) * D_8009FAA4) >> 10 decides between 0x1C and 0;
 *         for 0 < arg0 < 0x21 the value (arg0 * D_8009FAA4 >> 6) % 28;
 *         for negative arg0 the mirrored 27 - ((-arg0 * D_8009FAA4 >> 6) % 28).
 *
 * @par Confirmed
 * Pure arithmetic over the map scale word D_8009FAA4; the positive branch
 * wraps into 28 cells, the negative branch mirrors around cell 27.
 *
 * @par Likely role
 * The coordinate fold of the minimap family (func_8006531C, func_8006680C):
 * 28 cells per axis, the >= 0x21 band marks the off-map border stripe.
 *
 * @par Unknown
 * Why the far band returns the alternating constants 0x1C/0 instead of a
 * cell index.
 *
 * @par Callers
 * func_8006531C, func_8006680C.
 */
s32 func_80065744(s32 arg0)
{
    s32 v;

    if (arg0 == 0) {
        return 0;
    }
    if (arg0 >= 0x21) {
        arg0 -= 0x20;
        v = (arg0 * gDiscShadeByte) >> 10;
        return (v & 1) ? 0x1C : 0;
    }
    if (arg0 > 0) {
        v = (arg0 * gDiscShadeByte) >> 6;
        return v % 28;
    }
    v = ((-arg0) * gDiscShadeByte) >> 6;
    v = v % 28;
    return 27 - v;
}
