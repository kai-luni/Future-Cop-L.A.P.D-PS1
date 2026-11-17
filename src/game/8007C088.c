// Categories: Game Engine > Front-End Configuration | Input > Port Presets
#include "game/types.h"

extern u8 D_8009FA98;
extern u8 D_8009FA99;
extern u8 gRumbleEnabled __asm__("D_8009F8D8");
extern u8 D_8009F8D9;
extern u8 D_8009F900;
extern u8 D_8009F901;
extern u8 D_8009F902;
extern u8 D_8009F903;
extern s8 D_8009F904;
extern u8 D_800A374D[];
extern u8 func_8001EB2C(s32 arg0);

/**
 * @brief Snapshots both ports' controller presets and actuator flags when opening the port setup menu.
 *
 * Confirmed: copies active preset bytes D_8009FA98 and D_8009FA99 into D_8009F900 and D_8009F901, reads both ports' rumble-enable flags via func_8001EB2C(0) and func_8001EB2C(1) into D_8009F8D8 and D_8009F8D9, clears D_8009F902 and D_8009F903, and sets D_800A374D[0] (D_800A3748.unk05) to 3 when D_8009F904 == 1 else 1.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009FA98/D_8009FA99 -> D_8009F900/D_8009F901 (preset snapshots); D_8009F8D8/D_8009F8D9 (rumble flags); D_8009F902/D_8009F903 (cleared); D_8009F904 (port selector read); D_800A374D (menu mode byte written).
 *
 * @par Control flow
 * Straight-line global snapshot and two func_8001EB2C calls followed by an if/else on D_8009F904 == 1.
 *
 * @par Calls
 * func_8001EB2C - reads PadActuator enabled byte (+0x17 in D_800A6FF8) for ports 0 and 1.
 *
 * @par Side effects
 * Overwrites D_8009F900..D_8009F903, D_8009F8D8, D_8009F8D9, and D_800A374D[0] (D_800A3748 +0x05).
 *
 * @par Callers
 * Confirmed: address taken by func_80077AD4 (0x80077D28 in asm/us/main.s, stored into D_800A3748.unk10) / Likely: invoked indirectly via D_8009F468->unk10() in func_80078650 / Unknown: other indirect callers.
 */
void func_8007C088(void)
{
    D_8009F900 = D_8009FA98;
    D_8009F901 = D_8009FA99;
    gRumbleEnabled = func_8001EB2C(0);
    D_8009F8D9 = func_8001EB2C(1);
    D_8009F902 = 0;
    D_8009F903 = 0;
    if (D_8009F904 == 1) {
        D_800A374D[0] = 3;
    } else {
        D_800A374D[0] = 1;
    }
}
