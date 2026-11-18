// Categories: Game Engine > Controller Setup | Platform > Controller Input
#include "game/types.h"

extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern s8 gSnapshotResult __asm__("D_8009F904");
extern s8 D_8009F900[];
extern u8 gRumbleEnabled[] __asm__("D_8009F8D8");
extern u8 D_8009F902[];

extern void gApplyControllerPreset(s32 port, s32 value) __asm__("func_8007BFCC");
extern void func_8001EC04(s32 port, s32 enable);
extern s32 func_80046C84(s32 id, s32 arg1);

/**
 * @brief Handles one navigation action of the controller configuration screen.
 *
 * The function only acts on the port stored in D_8009F8B8 while that port is
 * the one being configured (D_8009F904). Action 3 applies the current choice:
 * func_8007BFCC writes the selected mode and func_8001EC04 restores the saved
 * motor setting. Actions 4 and 5 start sound 0x46 and flip the per-port
 * toggle. Actions 1 and 2 step the four-entry selector up or down with
 * wraparound, each with sound 0x47.
 *
 * @param action 1 down, 2 up, 3 confirm, 4/5 toggle.
 *
 * @par Globals
 * D_8009F8B8 - selected port; D_8009F904 - configured port;
 * D_8009F900 - per-port selector; D_8009F902 - per-port toggle;
 * D_8009F8D8 - saved motor mode.
 *
 * @par Control flow
 * Guard on the active port, then a switch with five cases.
 *
 * @par Calls
 * func_8007BFCC - applies the port mode. func_8001EC04 - restores the motor.
 * func_80046C84 - UI sounds.
 *
 * @par Side effects
 * Writes the selector and toggle bytes and the pad motor state.
 *
 * @par Callers
 * Unknown: no direct edge; reached from the menu dispatcher.
 */
void gHandleControllerSetupAction(s32 action) __asm__("func_8007C80C");

void gHandleControllerSetupAction(s32 action)
{
    s32 port;

    port = gActivePlayerIndex;
    if (port == gSnapshotResult) {
        switch (action) {
        case 3:
            gApplyControllerPreset(port, D_8009F900[port]);
            func_8001EC04(port, gRumbleEnabled[port]);
            break;
        case 4:
        case 5:
            func_80046C84(0x46, 0);
            D_8009F902[port] ^= 1;
            break;
        case 1:
            D_8009F900[port]++;
            if (D_8009F900[port] >= 4) {
                D_8009F900[port] = 0;
            }
            func_80046C84(0x47, 0);
            break;
        case 2:
            D_8009F900[port]--;
            if (D_8009F900[port] < 0) {
                D_8009F900[port] = 3;
            }
            func_80046C84(0x47, 0);
            break;
        }
    }
}
