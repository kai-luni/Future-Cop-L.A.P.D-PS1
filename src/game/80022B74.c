// Categories: Game Engine > State
#include "../../include/game/types.h"

extern s32 gStreamAvailableBlocks __asm__("D_800A09DC");

s32 gGetAudioStreamBlockCount(void) __asm__("func_80022B74");

/**
 * @brief Returns the audio stream's current block count.
 *
 * @param void No parameters.
 * @return The current value of gStreamAvailableBlocks.
 *
 * @par Confirmed mechanics
 * Loads the word at mixer +0x74 into v0. Stream reset initializes the count from +0x70, the IRQ
 * step increments it up to that limit and transfer submission decrements it.
 *
 * @par Globals
 * Reads gStreamAvailableBlocks, which aliases gMixerState +0x74.
 *
 * @par Callers
 * Direct calls from func_8002443C.
 *
 * @par Role and path
 * The event callback calculates a scaled value from the difference between three and this
 * count.
 *
 * @par Unknown
 * The value represents ring bookkeeping, and this getter does not convert it to bytes or ticks.
 */
s32 gGetAudioStreamBlockCount(void)
{
    return gStreamAvailableBlocks;
}
