// Categories: Utilities > Mathematics > Trigonometry
#include "game/types.h"

extern u16 gSineTable[] __asm__("D_8009A234");
s32 gSineLookup(u32 angle) __asm__("func_80018534");


/**
 * @brief Sine lookup over a 1024-entry quarter-wave table, 12-bit angle.
 *
 * @param angle Angle in 12-bit units (masked to 0xFFF).
 * @return The sine value: table[index] << 1, negated in the second half
 *         (bit 0x800); the quarter fold mirrors index at 0x400.
 *
 * @par Confirmed
 * index = angle & 0x3FF; bit 0x400 mirrors it (0x400 - index); the table
 * D_8009A234 value is doubled; bit 0x800 negates. Pure lookup, no writes.
 *
 * @par Likely role
 * The wave source of the minimap background generator func_800654F8 (the
 * sine moire); the << 1 scaling matches the 0..255 byte range the generator
 * clamps to.
 *
 * @par Callers
 * func_800654F8.
 *
 * @par Environment path
 * Sine lookup over the quarter-wave table gSineTable: the angle is masked to
 * 12 bits, the index is mirrored as 0x400 - index in the second and fourth
 * quadrants (so entries 0..0x400 are used), the value is doubled and negated
 * when bit 0x800 is set. Its only caller is gBuildTilePattern
 * (func_800654F8), the 256-byte map tile pattern generator. It writes
 * nothing.
 */
s32 gSineLookup(u32 angle)
{
    s32 index;
    s32 value;

    angle &= 0xFFF;
    index = angle & 0x3FF;
    if (angle & 0x400) {
        index = 0x400 - index;
    }
    value = gSineTable[index] << 1;
    if (angle & 0x800) {
        value = -value;
    }
    return value;
}
