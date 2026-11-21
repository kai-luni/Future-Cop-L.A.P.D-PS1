// Categories: Platform > Sound > Volume
#include "game/types.h"

extern s32 gMusicVolumeSetting __asm__("D_8009F334");
s32 gGetVagmVolumeSetting(void) __asm__("func_80045288");


/**
 * @brief Returns the second volume setting (D_8009F334).
 *
 * @return The stored volume word.
 *
 * @par Confirmed
 * Pure getter of D_8009F334.
 *
 * @par Likely role
 * The SE volume getter: second gauge of the options screen, also read by
 * the sound init paths (func_8002A16C, func_80080984); address taken by
 * D_8007D18C.
 *
 * @par Callers
 * func_80025F58, func_800260B8, func_8002A16C, func_8007CA34, func_8007CACC,
 * func_8007CFA0, func_80080984, func_80080A5C; address referenced by
 * D_8007D18C.
 */
s32 gGetVagmVolumeSetting(void)
{
    return gMusicVolumeSetting;
}
