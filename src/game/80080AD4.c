// Categories: Resources > Save Data | Platform > Sound | Input > Port Presets
#include "game/types.h"

extern u8 *gSaveProfileImage __asm__("D_8009F92C");
extern void func_80080984(u8 *arg0, s32 arg1);

/**
 * @brief Restores audio volumes and controller presets from the movie/menu context when flagged.
 *
 * Reads D_8009F92C[0x923]; if non-zero, forwards the saved option sub-block at D_8009F92C + 0x40C
 * to func_80080984 with argument 1 so saved volumes, device enables, and port presets take effect.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F92C - movie/menu context pointer (+0x923 non-zero gate byte, +0x40C option sub-block).
 *
 * @par Control flow
 * Single branch on D_8009F92C[0x923] != 0; skips the call when zero.
 *
 * @par Calls
 * func_80080984 - applies the option sub-block at D_8009F92C + 0x40C (volumes, ports, stream state).
 *
 * @par Side effects
 * Indirectly updates audio mixer settings, input port presets, and stream playback via func_80080984.
 *
 * @par Callers
 * Confirmed: func_8007FB04 (0x8007FD0C, when D_8009F464 equals 2 in movie sub-state 1) / Likely: none / Unknown: none.
 */
void func_80080AD4(void)
{
    u8 *base = gSaveProfileImage;

    if (base[0x923] != 0) {
        func_80080984(base + 0x40C, 1);
    }
}
