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

void gArmAudioStreamSpuIrq(void) __asm__("func_80022F44");

/**
 * @brief Programs and enables the SPU interrupt for the current audio stream position.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Adds mixer +0x44 to +0x7C, passes the sum to func_80098BB8 to program register 0xD2 and calls
 * gSpuSetIrqEnable with 1.
 *
 * @par Globals
 * Reads the SPU buffer base and offset in gMixerState. Callees access the SPU registers.
 *
 * @par Callers
 * Direct calls from gStartAudioStreamChannels (func_800228F4).
 *
 * @par Role and path
 * Stream startup arms the first hardware interrupt after installing the callback.
 *
 * @par Unknown
 * The register helper rejects out-of-range address values.
 */
void gArmAudioStreamSpuIrq(void)
{
    func_80098BB8(gMixerState.base + gMixerState.offset);
    gSpuSetIrqEnable(1);
}
