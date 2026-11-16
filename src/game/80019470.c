// Categories: Utilities > Mathematics > Matrix | Graphics > GTE > Matrix Build
#include "game/types.h"

struct Matrix32 {
    s32 m[3][3];
};

/**
 * @brief Transposes a 3x3 32-bit matrix from source into destination.
 *
 * Copies the nine s32 elements of source into destination with row and column indices swapped (e.g. for building the inverse view matrix R^T in func_80018860 or rotating collision/object frames).
 *
 * @param source Input 3x3 s32 matrix; must not alias destination.
 * @param destination Output 3x3 s32 matrix receiving source^T.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line sequence of nine element assignments; no branches or loops.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Overwrites all 9 s32 words of destination.
 *
 * @par Callers
 * Confirmed: func_80016A64 (0x80016B0C), func_80017058 (0x80017100), func_80018860 (0x80018874), func_80031238 (0x800312F8), func_80051EB8 (0x80051FD0), and func_80061CAC (0x80061E7C).
 */
void func_80019470(struct Matrix32 *source, struct Matrix32 *destination)
{
    destination->m[0][0] = source->m[0][0];
    destination->m[0][1] = source->m[1][0];
    destination->m[0][2] = source->m[2][0];
    destination->m[1][0] = source->m[0][1];
    destination->m[1][1] = source->m[1][1];
    destination->m[1][2] = source->m[2][1];
    destination->m[2][0] = source->m[0][2];
    destination->m[2][1] = source->m[1][2];
    destination->m[2][2] = source->m[2][2];
}
