// Categories: Graphics > Video Decode | Platform > MDEC
#include "game/types.h"

extern void func_80096240(u32 *arg0, u16 arg1);

/**
 * @brief Sets or clears two control bits of the decode control word and
 *        commits the result.
 *
 * @param arg0 Pointer to the control word.
 * @param arg1 Bit selector: bit 0 controls 0x08000000 (set when 0, cleared
 *             when 1), bit 1 controls 0x02000000 (set when 1, cleared
 *             when 0).
 * @return Nothing.
 *
 * @par Confirmed
 * The two bits are written with opposite polarity, then
 * func_80096240(arg0, *(u16 *)arg0) commits the word together with its
 * low halfword.
 *
 * @par Unknown
 * The exact meaning of the two bits (output format / signedness of the
 * MDEC path) is not proven from this body alone.
 *
 * @par Callers
 * func_80085228, func_80085630, func_800861E4.
 */
void func_80095FF4(u32 *arg0, s32 arg1)
{
    if (arg1 & 1) {
        *arg0 &= 0xF7FFFFFF;
    } else {
        *arg0 |= 0x08000000;
    }

    if (arg1 & 2) {
        *arg0 |= 0x02000000;
    } else {
        *arg0 &= 0xFDFFFFFF;
    }

    func_80096240(arg0, *(u16 *)arg0);
}
