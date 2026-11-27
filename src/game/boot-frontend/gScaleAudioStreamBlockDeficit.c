// Categories: Game Engine > State
#include "game/types.h"

extern int gGetAudioStreamBlockCount() __asm__("func_80022B74");

extern s32 gStreamBlockDeficitScale __asm__("D_8009F64C");

s32 gScaleAudioStreamBlockDeficit(void) __asm__("func_8002443C");

/**
 * @brief Multiplies three minus the current audio stream block count by the stream scale.
 *
 * @param void No parameters.
 * @return The computed signed product.
 *
 * @par Confirmed mechanics
 * Calls gGetAudioStreamBlockCount, subtracts the result from 3, multiplies by the word at gp
 * +0x438 and returns LO.
 *
 * @par Globals
 * Reads gStreamBlockDeficitScale and indirectly reads gStreamAvailableBlocks.
 *
 * @par Callers
 * Direct calls from func_80029BE4.
 *
 * @par Role and path
 * The stream control path uses the scaled deficit to decide when it can deliver more data.
 *
 * @par Unknown
 * The product is returned as a signed word and its units depend on the configured scale.
 */
s32 gScaleAudioStreamBlockDeficit(void)
{
    return gStreamBlockDeficitScale * (3 - gGetAudioStreamBlockCount());
}
