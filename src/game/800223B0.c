// Categories: Game Engine > Audio
#include "game/types.h"

struct State { u8 pad[0x70]; s32 scale; };
extern struct State gSoundSystem __asm__("D_800A07A8");

/**
 *  @brief Calculate two scaled channel values from a level and pan control.
 *
 *  The low seven pan bits determine attenuation around center 0x40, while
 *  the high bit negates the first channel. Optional bytes apply per-channel
 *  scaling before the shared scale is applied.
 *  @param a0 Optional byte array whose elements 1 and 2 scale the channels.
 *  @param a1 Base level used for both channels.
 *  @param a2 Pan control, including the first-channel sign bit.
 *  @param a3 Receives the first scaled channel value.
 *  @param a4 Receives the second scaled channel value.
 *  @par Globals
 *  D_800A07A8.scale supplies the final multiplier.
 *  @par Control flow
 *  Branches on the sign bit and on whether the optional array is present.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes the two output values through a3 and a4.
 *  @par Callers
 *  Confirmed: func_80021F98, func_800220C4, func_800222CC.
 */
void func_800223B0(u8 *a0, s32 a1, s32 a2, s32 *a3, s32 *a4)
{
    s32 values[2];
    s32 product = ((a2 & 0x7f) - 0x40) * a1;
    s32 pan = product >> 6;
    values[0] = a1 - (pan & (-pan >> 31));
    values[1] = a1 + (pan & (product >> 31));
    if (a2 & 0x80)
        values[0] = -values[0];
    if (a0) {
        values[0] = (values[0] * a0[1]) >> 7;
        values[1] = (values[1] * a0[2]) >> 7;
    }
    *a3 = values[0] * gSoundSystem.scale;
    *a4 = values[1] * gSoundSystem.scale;
}
