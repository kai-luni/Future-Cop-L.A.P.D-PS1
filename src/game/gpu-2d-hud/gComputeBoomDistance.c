// Categories: Graphics > Camera > Distance
#include "game/types.h"
s32 gComputeBoomDistance(s32 arg0) __asm__("func_800564A8");


/**
 * @brief Maps a camera parameter through a quadratic curve to a boom
 *        distance, clamped at a minimum.
 *
 * @param arg0 Input parameter (offset 0x14A, scaled by 600).
 * @return The distance word, at least 0x9000.
 *
 * @par Confirmed
 * value = 0x10000 - ((arg0 - 0x14A) << 16) / 600; then r = 0x20000 *
 * (0x10000 - value^2) in 16.16 arithmetic, plus 0xD800; results <= 0x8FFF
 * clamp to 0x9000.
 *
 * @par Likely role
 * The lazy distance resolve of the camera presets (func_800569EC,
 * func_800573A4): the quadratic term gives an ease-in/out boom length over
 * the parameter range, the floor keeps the camera out of the model.
 *
 * @par Unknown
 * What the input parameter measures (likely pitch or zoom level).
 *
 * @par Callers
 * func_800569EC, func_800573A4.
 *
 * @par Environment path
 * Maps a pitch angle to a camera distance: value = 0x10000 - ((pitch - 0x14A)
 * << 16) / 600, then 0x20000 * (0x10000 - value^2) in 16.16 arithmetic plus
 * 0xD800, with results at or below 0x8FFF raised to 0x9000. The chase-camera
 * step func_800569EC uses it for the boom length after smoothing the pitch,
 * and the actor reset func_800573A4 uses it to fill the distance of each
 * camera preset entry from that entry's pitch. It reads no globals and has no
 * side effects.
 */
s32 gComputeBoomDistance(s32 arg0)
{
    register u32 lo0 __asm__("$8");
    register u32 hi0 __asm__("$9");
    register s32 value __asm__("$2");
    register s32 r __asm__("$3");

    value = 0x10000 - (((arg0 - 0x14A) << 16) / 600);
    __asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(value), "r"(value));
    hi0 <<= 16;
    lo0 >>= 16;
    value = 0x10000 - (hi0 | lo0);
    __asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(0x20000), "r"(value));
    hi0 <<= 16;
    lo0 >>= 16;
    r = hi0 | lo0;
    r += 0xD800;
    if (r <= 0x8FFF) {
        r = 0x9000;
    }
    return r;
}
