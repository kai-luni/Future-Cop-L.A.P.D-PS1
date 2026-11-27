// Categories: Platform > Sound | Platform > DMA
#include "game/types.h"

extern s32 gNormalisedMode __asm__("D_8009EA34");
extern s32 gOpOrderFlag __asm__("D_8009EEC0");

/**
 * @brief Reads the SPU DMA enable flag and normalises the mode word to 0/1.
 *
 * @return The normalised mode (D_8009EA34 after the call).
 *
 * @par Confirmed
 * Reads D_8009EEC0; when it is 1 the mode word D_8009EA34 is set to 1,
 * otherwise to 0, and the result is returned. No other side effect.
 *
 * @par Likely role
 * The query side of gSpuSetTransferMode: the asset driver func_800211FC checks
 * whether the sound DMA is currently enabled before queueing more traffic.
 *
 * @par Callers
 * func_800211FC.
 */
s32 gSpuGetTransferMode(void) __asm__("func_80098F78");

s32 gSpuGetTransferMode(void)
{
    s32 mode;

    mode = gOpOrderFlag;
    switch (mode) {
    case 1:
        gNormalisedMode = mode;
        break;
    case 0:
    default:
        gNormalisedMode = 0;
        break;
    }

    return gNormalisedMode;
}
