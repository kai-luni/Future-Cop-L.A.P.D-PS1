// Categories: Platform > Streaming Audio | Platform > Sound
#include "game/types.h"

extern u32 gCallGateFlag __asm__("D_8009F6DC");

/**
 * @brief Returns whether audio stream playback state D_8009F6DC is active (non-zero).
 *
 * Evaluates D_8009F6DC != 0 as a 0/1 boolean. Options and save-restore paths check this alongside func_80045288 to decide whether to start or stop streaming.
 *
 * @param void No parameters.
 * @return 1 if D_8009F6DC != 0, 0 otherwise.
 *
 * @par Globals
 * D_8009F6DC - audio stream playback state word, read-only.
 *
 * @par Control flow
 * Straight-line comparison and return; no branches or loops.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: D_8007D18C (0x8007D1F0), func_8007CA34 (0x8007CA9C), func_80080984 (0x80080A04, 0x80080A2C).
 */
s32 func_8002A58C(void)
{
    return gCallGateFlag != 0;
}
