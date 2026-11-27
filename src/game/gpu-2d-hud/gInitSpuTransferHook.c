// Categories: Graphics > Sprite/Asset Subsystem | Platform > Interrupts
#include "game/types.h"

extern s32 gSavedSpuTransferCallback __asm__("D_8009F9AC");
extern s32 gSavedSpuTransferMode __asm__("D_8009FA14");
extern s32 gSlotIndexLimit __asm__("D_8009FA80");
extern s32 gSpuTransferWriteIndex __asm__("D_8009F998");
extern s32 gSpuTransferActive __asm__("D_8009FA34");
extern s32 gSpuTransferDispatching __asm__("D_8009FAD0");
extern s32 gInitDoneFlag __asm__("D_8009F9C8");

extern void gPerFrameHook(void) __asm__("D_8002139C");
extern s32 gSpuSetTransferCallback(void (*callback)(void)) __asm__("func_80098FB8");
extern s32 gSpuGetTransferMode(void) __asm__("func_80098F78");
extern void gSpuSetTransferMode(s32 value) __asm__("func_80098F48");
void gInitSpuTransferHook(void) __asm__("func_800211FC");


/**
 * @brief Initializes the subsystem's interrupt side: installs the
 *        callback, queries the mode and clears the state words.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * D_8009F9AC = gSpuSetTransferCallback(D_8002139C) - the callee stores the callback
 * address in D_8009EEDC; D_8009FA14 = gSpuGetTransferMode() (mode query over
 * D_8009EEC0); gSpuSetTransferMode(0) (disable form). D_8009FA80, D_8009F998,
 * D_8009FA34, D_8009FAD0 are zeroed, D_8009F9C8 = 1.
 *
 * @par Likely role
 * D_8002139C is the per-frame hook the interrupt path dispatches; the
 * state words gate the asset/sprite processing.
 *
 * @par Callers
 * func_8002333C.
 *
 * @par Environment path
 * Not on the draw path: it hooks the transfer callback of the sound side. It
 * installs the hook at 0x8002139C via func_80098FB8, keeps the previous
 * callback in D_8009F9AC and the previous mode in D_8009FA14, sets mode 0
 * through func_80098F48, zeroes four state words (gSlotIndexLimit, D_8009F998,
 * D_8009FA34, D_8009FAD0) and sets gInitDoneFlag to 1. Called by
 * func_8002333C; func_80021250 undoes it.
 */
void gInitSpuTransferHook(void)
{
    gSavedSpuTransferCallback = gSpuSetTransferCallback(gPerFrameHook);
    gSavedSpuTransferMode = gSpuGetTransferMode();
    gSpuSetTransferMode(0);
    gSlotIndexLimit = 0;
    gSpuTransferWriteIndex = 0;
    gSpuTransferActive = 0;
    gSpuTransferDispatching = 0;
    gInitDoneFlag = 1;
}
