// Categories: Utilities > Mathematics > Trigonometry | Graphics > Geometry
#include "game/types.h"

extern u16 gSineTable[] __asm__("D_8009A234");

/**
 * @brief Sine/cosine from the engine's 4096-step angle table.
 *
 * @param angle Angle in 1/4096 of a turn; the upper bits select the quadrant.
 * @param sine, cosine Out-parameters (16-bit values scaled by two).
 * @return Nothing; writes both out-parameters in every branch.
 *
 * @par Confirmed
 * angle &= 0xFFF, index = angle & 0x3FF, s = D_8009A234[index] << 1,
 * c = D_8009A234[0x400 - index] << 1, quadrant = angle >> 10, then the four
 * quadrant cases: (s, c), (c, -s), (-s, -c), (-c, s).
 *
 * @par Likely role
 * The table holds quarter-wave values (0x400 = 1024 entries for a quarter turn)
 * and the doubling converts its 15-bit entries to 16-bit.
 */
void func_8001846C(u32 angle, s32 *sine, s32 *cosine)
{
    s32 index;
    s32 s;
    s32 c;
    s32 quadrant;

    angle &= 0xFFF;
    index = angle & 0x3FF;
    s = gSineTable[index] << 1;
    c = gSineTable[0x400 - index] << 1;
    quadrant = (s32)angle >> 10;
    switch (quadrant) {
    case 0:
        *sine = s;
        *cosine = c;
        break;
    case 1:
        *sine = c;
        *cosine = -s;
        break;
    case 2:
        *sine = -s;
        *cosine = -c;
        break;
    case 3:
        *sine = -c;
        *cosine = s;
        break;
    }
}
