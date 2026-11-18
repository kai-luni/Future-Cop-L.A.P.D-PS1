// Categories: Resources > Save Data | Game Engine > State Machine
#include "game/types.h"

extern u8 *gSaveProfileImage __asm__("D_8009F92C");
extern s32 gStillRunningState __asm__("D_8009F464");

extern void func_8007FB04(void);

/**
 * @brief Pumps the boot profile transfer one step and releases the wait.
 *
 * Runs func_8007FB04() (one transfer step) and then looks at the transfer state
 * byte +0x200: 1 or 6 keeps the wait alive, 2 keeps it alive as well while the
 * sub-state +0x201 is 1 (load), so the loaded options are applied first; every
 * other combination sets D_8009F464 = 3, which ends the boot wait loop.
 *
 * @par Globals
 * D_8009F92C (profile context), D_8009F464 (boot transfer state).
 *
 * @par Callers
 * func_80076A2C only (boot wait loop).
 */
void gStepBootProfileTransfer(void) __asm__("func_80080B08");

void gStepBootProfileTransfer(void)
{
    u8 s;

    func_8007FB04();
    s = gSaveProfileImage[0x200];
    switch (s) {
    case 1:
    case 6:
        break;
    case 2:
        if (gSaveProfileImage[0x201] != 1) {
            gStillRunningState = 3;
        }
        break;
    default:
        gStillRunningState = 3;
        break;
    }
}
