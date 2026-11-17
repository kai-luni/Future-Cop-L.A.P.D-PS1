// Categories: Platform > Sound | Platform > DMA
#include "../../include/game/types.h"

extern s32 gNormalisedMode __asm__("D_8009EA34");
extern s32 gOpOrderFlag __asm__("D_8009EEC0");

/**
 * @brief Selects the SPU DMA transfer mode and its enable flag.
 *
 * @param mode 0 off, 1 on; any other value is stored but disables.
 * @return Nothing.
 *
 * @par Confirmed
 * D_8009EA34 takes the raw mode, D_8009EEC0 takes 1 only for mode == 1
 * (0 otherwise). Both words are read back by func_80098F78 and the transfer
 * paths.
 *
 * @par Likely role
 * The master switch of the sound DMA channel: the asset driver func_800211FC
 * and func_8008684C enable it before streaming samples.
 *
 * @par Callers
 * func_800211FC, func_8008684C.
 */
void func_80098F48(s32 mode)
{
    s32 enabled;

    switch (mode) {
    case 0:
        enabled = 0;
        break;
    case 1:
        enabled = 1;
        break;
    default:
        enabled = 0;
        break;
    }
    gNormalisedMode = mode;
    gOpOrderFlag = enabled;
}
