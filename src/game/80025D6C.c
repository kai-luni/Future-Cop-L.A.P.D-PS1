// Categories: Game Engine > Spatial Calculations
#include "game/types.h"

extern s16 D_8009FA44[];
extern s32 *D_8009FA58[];
extern s32 gPlayerCount __asm__("D_8009FA0C");

extern s32 func_8001A4E8(s32 *a, s32 *b);

/**
 *  @brief Computes a distance-based value for each registered entry.
 *
 *  Each output starts at 0x168 and decreases beyond distance 0x6000,
 *  with the result clamped to zero before storage in the output table.
 *  @param arg0 Reference passed to the distance calculation.
 *  @par Globals
 *  D_8009FA0C gives the count; D_8009FA58 supplies entry pointers.
 *  D_8009FA44 receives the resulting signed 16-bit values.
 *  @par Control flow
 *  Iterates over the entries only when the count is positive.
 *  Distances below 0x6000 retain the maximum value.
 *  @par Calls
 *  func_8001A4E8 calculates distance from arg0 to each entry at word offset 20.
 *  @par Side effects
 *  Overwrites one output-table element per processed entry.
 *  @par Callers
 *  Confirmed: func_8003E984.
 */
void func_80025D6C(s32 *arg0)
{
    s32 i;
    register s16 *out __asm__("$18");
    register s32 **in __asm__("$17");
    s32 value;
    register s32 distance __asm__("$3");

    i = 0;
    if (i < gPlayerCount) {
        out = D_8009FA44;
        in = D_8009FA58;
        do {
            distance = func_8001A4E8(arg0, *in + 20);
            if (distance >= 0x6000) { value = 0x168; value -= (distance - 0x6000) >> 8; if (value < 0) value = 0; } else { value = 0x168; }
            *out = value;
            out++;
            in++;
            i++;
        } while (i < gPlayerCount);
    }
}
