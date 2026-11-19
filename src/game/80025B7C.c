// Categories: Game Engine > Screen State Machine | Resources > Module Teardown
#include "game/types.h"

typedef struct {
    u8 unk00[0x10];
    s32 unk10;
    u8 unk14[0x2C];
    s32 unk40;
    s32 unk44;
    u8 unk48[0x18];
    s32 unk60;
} UnkFA94;

extern UnkFA94 *gGameSession __asm__("D_8009FA94");

extern s32 gUploadInProgress __asm__("D_8009F9C4");
extern s32 gFrameDelta __asm__("D_8009F9FC");
extern s32 gScreenState __asm__("D_8009FA00");
extern s32 gFrameTickBase __asm__("D_8009FA30");
extern s32 gScreenResetFlag __asm__("D_8009FA70");
extern s32 gFrameTime __asm__("D_8009FAA4");
extern void *jtbl_80010DDC[];

extern void func_80092C8C(s32 arg0);
extern void gStopAllPadRumble(void) __asm__("func_8001EB6C");
extern u8 func_80024170(void);
extern void func_80025348(void);
extern void func_80047690(u8 enable);
extern void func_80029DD8(void);
extern void func_80023BF8(void);
extern void gUnloadFrontEndSubsystems(s32 arg0) __asm__("func_800249BC");
extern void gResetMissionSubsystems(s32 arg0) __asm__("func_80024D14");

/**
 * @brief Closes a front-end screen: maps the module's exit state to the next
 *        screen state and unloads the module.
 *
 * @param arg0 Module handle/name passed on to the unloader.
 *
 * @return Nothing. The next state is written to D_8009FA94->unk10.
 *
 * @par Confirmed transition table (D_8009FA00 - 5, 10 entries)
 * - 5, 6 -> unk10 = 2
 * - 7    -> unk10 = 3
 * - 8    -> if D_8009FA70 == 0: D_8009FA00 = 10 and an inner loop runs until
 *           D_8009FA00 changes and func_80024170() returns 0, refreshing the
 *           frame time (D_8009F9FC = D_8009FAA4 - D_8009FA30) and calling
 *           func_80025348() each round; then unk10 = 0.
 * - 9    -> unk10 = 0 when unk40 == unk44, otherwise 1
 * - other -> unk10 = 4
 *
 * After the switch: func_80047690(0) releases the display, func_80029DD8() and
 * func_80023BF8() finish the render side, unk60 is cleared and the module is
 * unloaded through gUnloadFrontEndSubsystems(arg0) (UI mode) or gResetMissionSubsystems(arg0).
 *
 * @par Globals
 * D_8009FA00 (current state), D_8009FA70, D_8009F9FC/D_8009FAA4/D_8009FA30
 * (frame timing), D_8009F9C4 (loader selector), D_8009FA94 (state block:
 * unk10, unk40, unk44, unk60), jtbl_80010DDC (the jump table).
 *
 * @par Likely meaning of the states
 * 2/3 lead back to the loader or the next screen, 0 returns to the front-end
 * menu and 4 enters the interactive loop of func_80027EE4/func_80028618.
 *
 * @par Callers
 * func_800287F4, as the last step of every screen.
 */
void gFinishScreenModule(s32 arg0) __asm__("func_80025B7C");

void gFinishScreenModule(s32 arg0)
{
    register s32 state __asm__("$16");
    register s32 saved_arg __asm__("$17");
    register u32 index __asm__("$3");
    static void *const label_table[] __attribute__((used)) = {
        &&state_5, &&state_6, &&state_7, &&state_8, &&state_9,
        &&state_default, &&state_default, &&state_default,
        &&state_default, &&state_default
    };

    saved_arg = arg0;
    func_80092C8C(0);
    gStopAllPadRumble();

    index = (u32)gScreenState - 5;
    if (index >= 10) {
        goto state_default;
    }
    goto *jtbl_80010DDC[index];

state_5:
        gGameSession->unk10 = 2;
        goto state_done;
state_6:
        gGameSession->unk10 = 2;
        goto state_done;
state_7:
        gGameSession->unk10 = 3;
        goto state_done;
state_8:
        if (gScreenResetFlag == 0) {
            gScreenState = 10;
            state = gScreenState;
            while (1) {
                if (gScreenState != state && func_80024170() == 0) {
                    break;
                }
                gFrameDelta = gFrameTime - gFrameTickBase;
                gFrameTickBase = gFrameTime;
                func_80025348();
            }
        }
        gGameSession->unk10 = 0;
        goto state_done;
state_9:
        if (gGameSession->unk40 == gGameSession->unk44) {
            gGameSession->unk10 = 0;
        } else {
            gGameSession->unk10 = 1;
        }
        goto state_done;
state_default:
        gGameSession->unk10 = 4;

state_done:
    func_80047690(0);
    func_80029DD8();
    func_80023BF8();

    gGameSession->unk60 = 0;
    if (gUploadInProgress != 0) {
        gUnloadFrontEndSubsystems(saved_arg);
    } else {
        gResetMissionSubsystems(saved_arg);
    }
}
