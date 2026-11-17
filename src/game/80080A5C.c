// Categories: Resources > Save Data | Platform > Sound > Volume | Game Engine > Front End
#include "game/types.h"

extern s32 func_800451E0(void);
extern s32 func_8004522C(void);
extern s32 func_80045288(void);
extern s32 func_800452C8(void);
extern s32 func_8001EB2C(s32 arg0);

extern u8 gPresetIndexPerPort __asm__("D_8009FA98");
extern u8 D_8009FA99[];

/**
 * @brief Snapshots current audio volumes, option bytes, and device settings into a save/option sub-block.
 *
 * Reads four sound volume/mixer getters, two global option bytes, and two entries from the controller/device table via func_8001EB2C, packing the 8 resulting bytes into arg0 + 0x04..0x0B.
 *
 * @param arg0 Destination block; bytes +0x04..+0x0B are overwritten.
 *
 * @par Globals
 * D_8009FA98 - option byte written to arg0[8]. D_8009FA99 - byte array whose index 0 is written to arg0[9].
 *
 * @par Control flow
 * Straight-line sequence of six getter calls and two global byte loads; no branches or loops.
 *
 * @par Calls
 * func_8004522C, func_80045288, func_800452C8, func_800451E0 - supply music, SFX, stream, and fourth audio bytes for +0x04..+0x07.
 * func_8001EB2C - reads byte +0x17 from device/controller table slots 0 and 1 into +0x0A and +0x0B.
 *
 * @par Side effects
 * Overwrites arg0[4] through arg0[0xB].
 *
 * @par Callers
 * Confirmed: func_80080B70 (0x80080BB4 for header options at +0x40C and 0x80080BDC for profile record audio block at +0x474).
 */
void func_80080A5C(u8 *arg0)
{
    arg0[4] = func_8004522C();
    arg0[5] = func_80045288();
    arg0[6] = func_800452C8();
    arg0[7] = func_800451E0();
    arg0[0xA] = func_8001EB2C(0);
    arg0[0xB] = func_8001EB2C(1);
    arg0[8] = gPresetIndexPerPort;
    arg0[9] = D_8009FA99[0];
}
