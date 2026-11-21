// Categories: Graphics > Text > Font Upload
#include "game/types.h"

extern s32 gCurrentFontBankIndex __asm__("D_8009F76C");
extern s32 gFontBankCount __asm__("D_8009F364");
extern u16 gTexBankVramY __asm__("D_8009F76A");

extern void gLoadFontAsset(s32 value) __asm__("func_80048BF8");

/**
 * @brief Re-uploads font 0 when exactly one font is registered and the
 *        upload flag is clear.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Guard: D_8009F76C == 0 && D_8009F364 == 1 (the font counter). Then
 * D_8009F76A = D_8009F364 and func_80048BF8(0) uploads slot 0 to VRAM.
 *
 * @par Likely role
 * Restores the default font after a VRAM disturbance while no second
 * font is active.
 *
 * @par Callers
 * func_80063C5C.
 */
void func_80048DEC(void)
{
    if (gCurrentFontBankIndex == 0 && gFontBankCount == 1) {
        gTexBankVramY = gFontBankCount;
        gLoadFontAsset(0);
    }
}
