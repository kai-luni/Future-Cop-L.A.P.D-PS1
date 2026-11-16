// Categories: Platform > Sound | Platform > Device Shutdown
#include "game/types.h"

extern void func_80021F3C(void);
extern void func_80097A48(void);
extern void func_800216D8(void);

/**
 * @brief Shuts down the sound driver, SPU DMA event, and loaded sound banks.
 *
 * Runs the three-step audio teardown: func_80021F3C masks off and stops all registered channels in D_800A07A8, func_80097A48 clears the channel-4 SPU DMA callback and closes BIOS event D_8009EA2C, and func_800216D8 releases loaded sound banks and clears D_800A07A8 table pointers.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None directly; callees tear down D_800A07A8 and SPU DMA globals D_8009EE94/EEDC/EEE0/EA2C.
 *
 * @par Control flow
 * Three sequential calls (func_80021F3C, func_80097A48, func_800216D8) and return.
 *
 * @par Calls
 * func_80021F3C - builds active channel mask from D_800A07A8 and halts all channels via func_80021AB4/func_80021E1C.
 * func_80097A48 - clears SPU DMA channel-4 callback and closes/disables BIOS event D_8009EA2C.
 * func_800216D8 - releases sound banks via func_800217A8 and clears D_800A07A8 banks/channels.
 *
 * @par Side effects
 * Stops all sound channels, unhooks SPU DMA BIOS events, frees sound banks, and zeroes D_800A07A8 pointers.
 *
 * @par Callers
 * Confirmed: func_80021C20 (0x80021C60 in asm/us/main.s), func_80023478 (0x800234DC in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_80021C80(void)
{
    func_80021F3C();
    func_80097A48();
    func_800216D8();
}
