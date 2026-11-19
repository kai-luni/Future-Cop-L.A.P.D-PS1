// Categories: Game Engine > Dispatch
#include "game/types.h"

extern void gPollAudioStreamStop(void) __asm__("func_80022654");

void gServiceAudioStreamStop(void) __asm__("func_8002441C");

/**
 * @brief Services the stream's pending-stop and deadline checks.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Calls gPollAudioStreamStop and returns without further dispatch.
 *
 * @par Globals
 * None directly. The callee reads mixer state and the frame tick.
 *
 * @par Callers
 * Direct calls from func_800462D0.
 *
 * @par Role and path
 * The audio update path invokes this wrapper to service deferred stream stops.
 *
 * @par Unknown
 * The hook adds no decisions of its own.
 */
void gServiceAudioStreamStop(void)
{
    gPollAudioStreamStop();
}
