// Categories: Gameplay > Path Following | Gameplay > Map
#include "game/types.h"

struct Unk50DACCell {
    s16 unk00;
    s16 x;
    s16 unk04;
    s16 z;
    s32 unk08;
};

struct Unk50DACMap {
    s16 unk00;
    s16 count;
    s32 unk04;
    struct Unk50DACCell cells[1];
};

/**
 * @brief Finds the index of the map path cell containing the given fixed-point X/Z world coordinates.
 *
 * Confirmed: shifts position[0] and position[2] right by 8 bits to convert 8-bit fixed-point coordinates to 16-bit cell coordinates and linearly searches map->cells[0..map->count - 1] (12-byte stride, s16 x at +0x02, s16 z at +0x06) for an exact match. Used by func_80050E0C to seat rail/spline movers.
 *
 * @param map Path map header (+0x02 s16 count, +0x08 cells[] array of 12-byte Unk50DACCell entries).
 * @param position Three-word position vector; reads word 0 (X) and word 2 (Z).
 * @return Zero-based cell index on match, or -1 if no cell matches.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Linear for loop over 0 <= i < map->count with immediate return on cell->x == x && cell->z == z, falling through to -1.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_80050E0C (0x80050E60 in asm/us/main.s) / Likely: none / Unknown: none.
 */
s32 func_80050DAC(struct Unk50DACMap *map, s32 *position)
{
    struct Unk50DACCell *cell;
    s32 x;
    s32 z;
    s32 i;

    x = position[0] >> 8;
    z = position[2] >> 8;
    cell = map->cells;
    for (i = 0; i < map->count; i++, cell++) {
        if (cell->x == x && cell->z == z) {
            return i;
        }
    }
    return -1;
}
