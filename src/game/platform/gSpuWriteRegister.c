// Categories: Platform > SPU | Platform > Sound > Init
#include "game/spu_globals.h"

#include "game/types.h"

extern u16 *gSpuRegBasePtr __asm__("D_8009EEA4");
extern u32 gSpuAddrShift __asm__("D_8009EECC");

/**
 * @brief Writes a 16-bit value to an SPU hardware register, optionally shifting by D_8009EECC.
 *
 * Indexes the SPU MMIO halfword register array D_8009EEA4 (0x1F801C00) at index: writes
 * value directly when flag is 0, or value >> D_8009EECC when flag is non-zero.
 *
 * @param index Halfword register index into D_8009EEA4 (e.g. 0xD1 for SPU RAM arena top).
 * @param value Value to store (written verbatim when flag == 0, else shifted right by D_8009EECC).
 * @param flag Shift mode: 0 writes raw value; non-zero shifts right by D_8009EECC.
 *
 * @par Globals
 * D_8009EEA4 - SPU MMIO base pointer (written at [index]); D_8009EECC - SPU address shift count.
 *
 * @par Control flow
 * Single branch on flag == 0 selecting raw store vs shifted store.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes one 16-bit SPU hardware register at D_8009EEA4[index].
 *
 * @par Callers
 * Confirmed: gSpuInitSubsystem (0x80096DE0, writes D_8009EF14 to register 0xD1 with flag 0) / Likely: none / Unknown: none.
 */
void gSpuWriteRegister(s32 index, u32 value, s32 flag) __asm__("func_800977C4");

void gSpuWriteRegister(s32 index, u32 value, s32 flag)
{
    if (flag == 0) {
        gSpuRegBasePtr[index] = value;
    } else {
        gSpuRegBasePtr[index] = value >> gSpuAddrShift;
    }
}
