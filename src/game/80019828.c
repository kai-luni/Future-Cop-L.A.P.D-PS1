// Categories: Utilities > Mathematics > Tables | Game Engine > Game State > Init
#include "game/types.h"

extern u16 *gReciprocalTablePtr __asm__("D_8009F5B0");

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern u32 func_80018818(u32 divisor);
extern u32 func_800187C8(u32 value);
extern s32 func_80095C14(u32 value);
extern void gSeedPrng(s32 seed) __asm__("func_800197A8");

/**
 * @brief Builds the 1024-entry reciprocal/mantissa table D_8009F5B0 and
 *        seeds the RNG.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Allocates 0x800 bytes from pool tag 2. For input 0..0x3FF: value =
 * func_800187C8(func_80018818(input)); shift = 0x15 minus the leading-zero
 * count (func_80095C14) when value != 0; value >>= shift (when positive),
 * then entry = (value << 5) | shift. Finally gSeedPrng(0x457) seeds
 * the RNG.
 *
 * @par Likely role
 * The table init behind the reciprocal-root helper func_80095900: each
 * entry packs a 5-bit exponent with the mantissa, the fixed seed 0x457
 * makes the boot sequence deterministic (the mission init re-seeds from
 * the saved value afterwards).
 *
 * @par Unknown
 * The exact transform of func_80018818/func_800187C8.
 *
 * @par Callers
 * func_80024904, func_80024A24.
 */
void gInitReciprocalTable(void) __asm__("func_80019828");

void gInitReciprocalTable(void)
{
    s32 i;
    u32 input;
    u32 value;
    s32 shift;

    gReciprocalTablePtr = gAllocMemory(0x800, 2);
    for (input = 0, i = input; i < 0x400; input++) {
        value = func_800187C8(func_80018818(input));
        shift = 0x15;
        if (value != 0) {
            shift -= func_80095C14(value);
        }
        if (shift > 0) {
            value >>= shift;
        }
        value <<= 5;
        value |= shift;
        gReciprocalTablePtr[i++] = value;
    }
    gSeedPrng(0x457);
}
