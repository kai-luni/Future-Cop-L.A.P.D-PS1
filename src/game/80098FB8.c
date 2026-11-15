// Categories: Platform > Sound | Platform > DMA
#include "../../include/game/types.h"

extern s32 D_8009EEDC;

/**
 * @brief Sets the SPU DMA state word, but only when the value changes.
 *
 * @param value New state for D_8009EEDC.
 * @return Nothing.
 *
 * @par Confirmed
 * Compare-and-write of D_8009EEDC; no other effect. The word sits in the
 * SPU DMA block next to the transfer callback D_8009EEE0 and is read by the
 * transfer paths func_80096EB8, func_800972F8, func_80097A48, func_80098E28.
 *
 * @par Likely role
 * The transfer-mode latch of the sound DMA: the sprite/asset driver
 * func_800211FC and the sound helpers func_80087750/func_80087824 switch it
 * before queueing SPU traffic.
 *
 * @par Unknown
 * The exact encoding of the state values.
 *
 * @par Callers
 * func_800211FC, func_80087750, func_80087824.
 */
void func_80098FB8(s32 value)
{
    if (value != D_8009EEDC) {
        D_8009EEDC = value;
    }
}
