// Categories: Platform > Audio
#include "game/types.h"

extern s32 gCurrentChannel __asm__("D_800A09F0");

extern void gSpuSetIrqEnable(s32 mode) __asm__("func_80098A78");

void gDisarmAudioStreamSpuIrq(void) __asm__("func_80022F7C");

/**
 * @brief Disables the stream SPU interrupt and resets the current channel marker.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Passes zero to gSpuSetIrqEnable and writes -1 to the channel word in the call delay slot.
 *
 * @par Globals
 * Writes gCurrentChannel. The callee clears and polls the IRQ-enable bit.
 *
 * @par Callers
 * Direct calls from gHandleAudioStreamSpuIrq (D_80022FA8), gStopAudioStreamPlayback
 * (func_80022A64).
 *
 * @par Role and path
 * The stream IRQ handler and the playback shutdown path disarm through this routine.
 *
 * @par Unknown
 * The poll duration depends on the hardware.
 */
void gDisarmAudioStreamSpuIrq(void)
{
    gCurrentChannel = -1;
    gSpuSetIrqEnable(0);
}
