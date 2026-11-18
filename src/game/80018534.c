// Categories: Utilities > Mathematics > Trigonometry
#include "game/types.h"

extern u16 gSineTable[] __asm__("D_8009A234");

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
 * Sine lookup over the 1024-entry quarter-wave table: the angle is masked to
 * 12 bits, the index is mirrored at 0x400, the table value is doubled and
 * negated when bit 0x800 is set. Its only caller is gBuildTilePattern
 * (func_800654F8, line 88), the 256-byte minimap/tileset pattern generator
 * that gDrawRegionTiles (func_8006531C, line 72) invokes whenever the tileset
 * id of a region changes. It is therefore a leaf of the environment tile
 * layer rather than of the model path: no actor or effect renderer calls it,
 * and it writes nothing.
 */
s32 func_80018534(u32 angle)
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
