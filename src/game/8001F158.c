// Categories: Game Engine > Timing | Platform > Interrupts
#include "game/types.h"

extern s32 D_8009F99C;
extern u8 D_8009FA20;

extern s32 func_80091510(void (*callback)(void));

/**
 * @brief Stops the per-frame callback driver started by func_8001F0B8.
 *
 * func_80091510(0) issues callback operation 4 with a null argument, the form that uninstalls the table func_8001F0B8 installed at D_8001EFF8; the hook word and the running flag are then cleared, so func_8001E540 skips this path on a second call.
 *
 * @return Nothing.
 *
 * @par Globals
 * D_8009F99C - per-frame hook pointer, cleared. D_8009FA20 - driver running flag, cleared.
 *
 * @par Control flow
 * Call, two stores, return; no branches.
 *
 * @par Calls
 * func_80091510 - callback operation 4 with a null argument (uninstall).
 *
 * @par Side effects
 * The frame hook stops running; the counters D_8009FA60 and D_8009FA2C keep their values and are only reset by the next start.
 *
 * @par Callers
 * Confirmed: func_8001E540 (0x8001E568), the controller/device shutdown, and only when D_8009FA20 reports the driver as running.
 */
void func_8001F158(void)
{
    func_80091510(0);
    D_8009F99C = 0;
    D_8009FA20 = 0;
}
