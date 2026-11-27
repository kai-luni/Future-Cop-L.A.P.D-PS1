// Categories: Platform > Sound > Volume
#include "game/types.h"

extern s32 gMusicVolume __asm__("D_8009F330");
s32 gGetMusicVolume(void) __asm__("func_8004522C");


/**
 * @brief Returns the first volume setting (D_8009F330).
 *
 * @return The stored volume word (0..127 range per the options scaling).
 *
 * @par Confirmed
 * Pure getter of D_8009F330.
 *
 * @par Likely role
 * The music volume getter: the options screen func_8007CFA0 shows it as
 * the first gauge and func_80025F58 formats it as a percentage; the
 * address is also taken by the front-end data block D_8007D18C.
 *
 * @par Callers
 * func_80025F58, func_800260B8, func_8007CACC, func_8007CFA0, func_80080A5C;
 * address referenced by D_8007D18C.
 */
s32 gGetMusicVolume(void)
{
    return gMusicVolume;
}
