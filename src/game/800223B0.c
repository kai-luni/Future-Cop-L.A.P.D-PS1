// Categories: Game Engine > Audio
#include "game/types.h"

struct State { u8 pad[0x70]; s32 scale; };
extern struct State gSoundSystem __asm__("D_800A07A8");

void gComputeStereoVoiceLevels(u8 *a0, s32 a1, s32 a2, s32 *a3, s32 *a4) __asm__("func_800223B0");

/**
 * @brief Computes left and right voice levels from a base level, pan and optional sample gains.
 *
 * @param a0 Optional byte array whose elements 1 and 2 scale the channels.
 * @param a1 Base level used for both channels.
 * @param a2 Pan control, including the first-channel sign bit.
 * @param a3 Receives the first scaled channel value.
 * @param a4 Receives the second scaled channel value.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Subtracts 0x40 from the seven-bit pan, multiplies by the base level and shifts right by six.
 * Sign masks select which side to attenuate. Bit 0x80 negates the left channel. Optional byte
 * gains at +1 and +2 are applied before both outputs are multiplied by the shared scale.
 *
 * @par Globals
 * Reads the final scale at gSoundSystem +0x70 and writes through the two output pointers.
 *
 * @par Callers
 * Direct calls from gQueueBankSampleOnChannel (func_80021F98), gAllocateSoundChannelVoice
 * (func_800220C4), gSetSoundChannelMix (func_800222CC).
 *
 * @par Role and path
 * Shared arithmetic for sound-bank samples, software voices and live SPU mix updates.
 *
 * @par Unknown
 * Inputs are multiplied without saturating the returned words.
 */
void gComputeStereoVoiceLevels(u8 *a0, s32 a1, s32 a2, s32 *a3, s32 *a4)
{
    s32 values[2];
    s32 product = ((a2 & 0x7f) - 0x40) * a1;
    s32 pan = product >> 6;
    values[0] = a1 - (pan & (-pan >> 31));
    values[1] = a1 + (pan & (product >> 31));
    if (a2 & 0x80)
        values[0] = -values[0];
    if (a0) {
        values[0] = (values[0] * a0[1]) >> 7;
        values[1] = (values[1] * a0[2]) >> 7;
    }
    *a3 = values[0] * gSoundSystem.scale;
    *a4 = values[1] * gSoundSystem.scale;
}
