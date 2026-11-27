// Categories: Game Engine > State
#include "game/types.h"

extern s32 gStripScaleEnable __asm__("D_8009F4C0");
extern s32 gStripDitherEnabled __asm__("D_8009F4C4");
extern s32 gStripStepWidth __asm__("D_8009F4C8");
extern s32 gBufferCount __asm__("D_8009F4CC");
extern s32 gReleaseDoneFlag __asm__("D_8009F508");
void gSetMovieStripDecodeDefaults(void) __asm__("func_800859B0");


/**
 *  @brief Initializes five global state values.
 *
 *  Assigns fixed values to a group of globals whose specific roles are not
 *  established by this function.
 *  @par Globals
 *  D_8009F4C0 and D_8009F508 are set to 1; D_8009F4C4 is set to 0.
 *  D_8009F4C8 and D_8009F4CC are set to 3.
 *  @par Control flow
 *  Performs the assignments unconditionally.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the five global values.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gSetMovieStripDecodeDefaults(void)
{
    gStripScaleEnable = 1;
    gStripStepWidth = 3;
    gBufferCount = 3;
    gStripDitherEnabled = 0;
    gReleaseDoneFlag = 1;
}
