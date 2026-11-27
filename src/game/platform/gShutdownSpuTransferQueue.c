// Categories: Platform > Audio | Game Engine > Request Queue
#include "game/types.h"

extern s32 gSavedSpuTransferCallback __asm__("D_8009F9AC");
extern s32 gSavedSpuTransferMode __asm__("D_8009FA14");
extern s32 gSpuTransferActive __asm__("D_8009FA34");
extern s32 gSpuTransferDispatching __asm__("D_8009FAD0");
extern s32 gInitDoneFlag __asm__("D_8009F9C8");

extern void gSpuCancelDmaRequests(void) __asm__("func_8002129C");
extern void gSpuSetTransferMode(s32 value) __asm__("func_80098F48");
extern s32 gSpuSetTransferCallback(s32 callback) __asm__("func_80098FB8");

void gShutdownSpuTransferQueue(void) __asm__("func_80021250");

/**
 * @brief Shuts down the SPU transfer queue and restores the previously saved
 * transfer settings.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * If gInitDoneFlag is nonzero, calls the queue cancel entry and restores
 * transfer mode then callback. Clears the active, dispatch-guard, and
 * initialization words on both paths.
 *
 * @par Globals
 * Reads gSavedSpuTransferMode and gSavedSpuTransferCallback. Clears
 * gSpuTransferActive, gSpuTransferDispatching, and gInitDoneFlag.
 *
 * @par Callers
 * Direct calls in the retail listing are from gShutdownCvkSystem.
 *
 * @par Role and path
 * The audio teardown path releases ownership of the SPU transfer callback.
 *
 * @par Unknown
 * The cancel routine controls how a transfer in flight is drained.
 */
void gShutdownSpuTransferQueue(void)
{
    if (gInitDoneFlag != 0) {
        gSpuCancelDmaRequests();
        gSpuSetTransferMode(gSavedSpuTransferMode);
        gSpuSetTransferCallback(gSavedSpuTransferCallback);
    }
    gSpuTransferActive = 0;
    gSpuTransferDispatching = 0;
    gInitDoneFlag = 0;
}
