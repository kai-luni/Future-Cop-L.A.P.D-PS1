// Categories: Game Engine > Spatial Calculations
#include "game/types.h"

extern s16 gPlayerFadeTimers[] __asm__("D_8009FA44");
extern s32 *gPlayerEntities[] __asm__("D_8009FA58");
extern s32 gPlayerCount __asm__("D_8009FA0C");

extern s32 func_8001A4E8(s32 *a, s32 *b);

void gSetPlayerFadeTimersByDistance(s32 *arg0) __asm__("func_80025D6C");

/**
 * @brief Sets each player's fade timer from the distance to a reference position.
 *
 * @param arg0 Reference passed to the distance calculation.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Calls func_8001A4E8 with the reference and each entity's +0x50 address. Stores 0x168 below
 * distance 0x6000, otherwise subtracts the excess shifted right by eight and clamps the result
 * to zero.
 *
 * @par Globals
 * Reads gPlayerCount and gPlayerEntities, writes gPlayerFadeTimers.
 *
 * @par Callers
 * Direct calls from gSpawnObjectFromTable (func_8003E984).
 *
 * @par Role and path
 * The object spawn path derives a per-player screen fade, which the frame fade routine later
 * drains.
 *
 * @par Unknown
 * The routine uses the caller's player pointers without null checks.
 */
void gSetPlayerFadeTimersByDistance(s32 *arg0)
{
    s32 i;
    register s16 *out __asm__("$18");
    register s32 **in __asm__("$17");
    s32 value;
    register s32 distance __asm__("$3");

    i = 0;
    if (i < gPlayerCount) {
        out = gPlayerFadeTimers;
        in = gPlayerEntities;
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
