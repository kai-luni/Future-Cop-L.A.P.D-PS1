// Categories: Graphics > Animation > Events
#include "game/types.h"

/**
 * @brief Packs a matching animation event code into a 6-bit-per-event
 *        bit accumulator.
 *
 * @param code Event word: low 10 bits the key, bits 10..15 the value.
 * @param key Key to match.
 * @param shift In/out bit cursor (capped below 0x1A = 26 bits).
 * @param bits In/out accumulator.
 * @return Nothing.
 *
 * @par Confirmed
 * Only when key == (code & 0x3FF) and *shift < 0x1A: the 6-bit value
 * (code >> 10) is ORed at *shift, the cursor advances by 6. Up to four
 * events fit (24 bits).
 *
 * @par Likely role
 * The event collector of the animation tick func_80044B34: each clip key
 * frame can carry several event codes (sound, effect, damage), packed
 * into one word for the handler.
 *
 * @par Callers
 * func_80044B34.
 */
void func_80044AE8(u32 code, u32 key, s32 *shift, u32 *bits)
{
    if (key == (code & 0x3FF) && *shift < 0x1A) {
        code &= 0xFC00;
        code >>= 10;
        code <<= *shift;
        *bits |= code;
        *shift += 6;
    }
}
