// Categories: Game Engine > State
#include "game/types.h"

extern s32 D_8009F9AC;
extern s32 D_8009FA14;
extern s32 D_8009FA34;
extern s32 D_8009FAD0;
extern s32 gInitDoneFlag __asm__("D_8009F9C8");

extern void gSpuCancelDmaRequests(void) __asm__("func_8002129C");
extern void gSpuSetTransferMode(s32 value) __asm__("func_80098F48");
extern s32 gSpuSetTransferCallback(s32 callback) __asm__("func_80098FB8");

/**
 *  @brief Performs conditional cleanup and clears three state flags.
 *
 *  When the active flag is set, the routine invokes cleanup and passes two
 *  saved values to other functions before resetting its local global state.
 *  @par Globals
 *  D_8009F9C8 gates cleanup and is cleared; D_8009FA14 and D_8009F9AC
 *  supply call arguments; D_8009FA34 and D_8009FAD0 are cleared.
 *  @par Control flow
 *  The three calls run only when D_8009F9C8 is nonzero; all three clears
 *  occur unconditionally afterward.
 *  @par Calls
 *  gSpuCancelDmaRequests performs cleanup; gSpuSetTransferMode and gSpuSetTransferCallback
 *  receive the saved values, with the latter's return value ignored.
 *  @par Side effects
 *  Clears three globals and may cause additional effects through the calls.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80021250(void)
{
    if (gInitDoneFlag != 0) {
        gSpuCancelDmaRequests();
        gSpuSetTransferMode(D_8009FA14);
        gSpuSetTransferCallback(D_8009F9AC);
    }
    D_8009FA34 = 0;
    D_8009FAD0 = 0;
    gInitDoneFlag = 0;
}
