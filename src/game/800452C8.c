// Categories: Platform > Sound > Volume
#include "game/types.h"

extern s32 D_8009F338;

/**
 * @brief Returns the third volume setting (D_8009F338).
 *
 * @return The stored volume word.
 *
 * @par Confirmed
 * Pure getter of D_8009F338.
 *
 * @par Likely role
 * The voice/stream volume getter: third gauge of the options screen, read
 * by the stream buffer setup func_80086C74 and the movie paths
 * (func_800298EC, func_80029974); address taken by D_8007D18C.
 *
 * @par Callers
 * func_80025F58, func_800260B8, func_800298EC, func_80029974, func_80045050,
 * func_8007CACC, func_8007CFA0, func_80080A5C, func_80086C74; address
 * referenced by D_8007D18C.
 */
s32 func_800452C8(void)
{
    return D_8009F338;
}
