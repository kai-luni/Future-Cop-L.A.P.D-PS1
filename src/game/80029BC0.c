// Categories: Platform > Streaming Audio | Platform > Sound
#include "game/types.h"

extern u32 gOutStreamId __asm__("D_8009F6BC");
extern u32 func_80045028(u32 arg);

/**
 * @brief Checks whether the active stream track D_8009F6BC matches D_800A1800.
 *
 * Passes D_8009F6BC (0x4A8($gp)) to func_80045028 and masks the result to 8 bits, returning 1 when D_800A1800 != -1 and equals D_8009F6BC.
 *
 * @param void No parameters.
 * @return 1 if D_800A1800 is active (!= -1) and equals D_8009F6BC, 0 otherwise.
 *
 * @par Globals
 * D_8009F6BC - active stream/track identifier read and passed to func_80045028.
 *
 * @par Control flow
 * Straight-line load of D_8009F6BC, call to func_80045028, and mask with 0xFF.
 *
 * @par Calls
 * func_80045028 - compares its argument against D_800A1800 when D_800A1800 != -1.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: gSoundPauseVoices (0x800476F0, decides whether to preserve the background stream voice on pause) / Likely: none / Unknown: none.
 */
u8 func_80029BC0(void)
{
    return func_80045028(gOutStreamId) & 0xFF;
}
