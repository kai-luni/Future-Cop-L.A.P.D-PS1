// Categories: Graphics > GTE > Projection | Platform > GTE
#include "game/types.h"

void gGteSetScreenOffset(u32 arg0, u32 arg1) __asm__("func_800959F0");

/**
 * @brief Sets the GTE screen offset (registers OFX = 24, OFY = 25).
 *
 * @param arg0 Horizontal offset in pixels, shifted left by 16 (fixed point
 *              16.16) before it is written to $24.
 * @param arg1 Vertical offset in pixels, same fixed-point conversion, into $25.
 * @return Nothing.
 *
 * @par Confirmed
 * The shifts by 16 are part of the retail code and are what the GTE expects:
 * OFX/OFY are 16.16 values whose integer part is the pixel offset.
 *
 * @par Callers
 * func_80010030 - the front-end passes (0xA0, 0x78) or (0x50, 0x78), i.e. the
 * centre of a 320x240 or 160x240 viewport at y = 120.
 */
void gGteSetScreenOffset(u32 arg0, u32 arg1) {
    arg0 <<= 16;
    arg1 <<= 16;
    __asm__ volatile("ctc2 %0, $24" : : "r"(arg0));
    __asm__ volatile("ctc2 %0, $25" : : "r"(arg1));
}
