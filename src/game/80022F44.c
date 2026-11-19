// Categories: Platform > Sound > Streaming | Platform > SPU Transfer
#include "game/types.h"

struct StreamState {
    u8 unk00[0x44];
    s32 base;
    u8 unk48[0x34];
    s32 offset;
};

extern struct StreamState gMixerState __asm__("D_800A0968");

extern void func_80098BB8(s32 address);
extern void gSpuSetIrqEnable(s32 mode) __asm__("func_80098A78");

/**
 * @brief Arms the SPU IRQ address and enables SPU interrupts for the active audio stream.
 *
 * Sets the SPU IRQ target address to D_800A0968.base + D_800A0968.offset via func_80098BB8 and
 * then enables and waits for bit 0x40 in the SPU control register via gSpuSetIrqEnable(1).
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_800A0968 - StreamState struct (reads +0x44 base SPU RAM address and +0x7C current offset).
 *
 * @par Control flow
 * Straight-line sequence calling func_80098BB8 followed by gSpuSetIrqEnable(1).
 *
 * @par Calls
 * func_80098BB8 - programs SPU IRQ address register 0xD2; gSpuSetIrqEnable - sets and polls SPU transfer flag bit 0x40.
 *
 * @par Side effects
 * Programs the SPU IRQ address and sets bit 0x40 in D_8009EEA4->flags via callees.
 *
 * @par Callers
 * Confirmed: gStartAudioStreamChannels (0x80022948, after installing callback D_80022FA8 during stream start) / Likely: none / Unknown: none.
 */
void func_80022F44(void)
{
    func_80098BB8(gMixerState.base + gMixerState.offset);
    gSpuSetIrqEnable(1);
}
