// Categories: Physics > World Bounds | Gameplay > Entity Movement
#include "game/types.h"

extern s32 D_800A2950[4];

/**
 * @brief Clamps an entity position's X and Z coordinates to the active map bounds.
 *
 * Reads the four map boundary limits from D_800A2950, applies an inward margin of
 * 0x800 units on each side, and clamps arg0[0] (X) and arg0[2] (Z) in place.
 *
 * @param arg0 Pointer to a 3-word s32 position vector (X at [0], Z at [2]).
 * @return Bitmask of clamped axes: bit 0 (1) if Z was clamped, bit 1 (2) if X was clamped.
 *
 * @par Globals
 * D_800A2950 - 4-word map boundary array [zMin, zMax, xMin, xMax] written by map setup.
 *
 * @par Control flow
 * Checks Z against [D_800A2950[0]+0x800, D_800A2950[1]-0x800], then X against
 * [D_800A2950[2]+0x800, D_800A2950[3]-0x800] via two if/else branches, setting result bits.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Updates arg0[0] and/or arg0[2] in place when out of bounds.
 *
 * @par Callers
 * Confirmed: func_80053A74, func_800549F0 (after integrating velocity into position).
 */
s32 func_8006943C(s32 *arg0)
{
    s32 lower;
    s32 z;
    s32 x;
    s32 changed;

    lower = D_800A2950[0] + 0x800;
    z = arg0[2];
    x = arg0[0];
    changed = 0;

    if (z < lower) {
        arg0[2] = lower;
        changed = 1;
    } else {
        lower = D_800A2950[1] - 0x800;
        if (lower < z) {
            arg0[2] = lower;
            changed = 1;
        }
    }

    lower = D_800A2950[2] + 0x800;
    if (x < lower) {
        arg0[0] = lower;
        changed |= 2;
    } else {
        lower = D_800A2950[3] - 0x800;
        if (lower < x) {
            arg0[0] = lower;
            changed |= 2;
        }
    }

    return changed;
}
