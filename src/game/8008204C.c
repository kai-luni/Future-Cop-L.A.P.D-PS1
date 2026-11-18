// Categories: Resources > Save Data | Platform > Memory Card
#include "game/types.h"

extern u8 *gSaveProfileImage __asm__("D_8009F92C");
extern u8 gUiHoldFlag __asm__("D_8009F8BD");
extern u8 gUiInteractionEnabled __asm__("D_8009F8BE");
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern u8 gPlayerMenuDepth[] __asm__("D_8009F8C0");

extern s32 func_80020B64(void);
extern void func_80091270(s32 arg0);
extern s32 func_800207B4(s32 arg0);
extern s32 func_80078740(void);

/**
 * @brief Runs the card format step of the profile transfer.
 *
 * While func_80020B64 reports an active format it waits one tick
 * (func_80091270(0)) and polls the card result code func_800207B4(0); any answer
 * other than -1 aborts after five iterations. When the format ends on its own -
 * or when none is running - the state bytes [0x922], [0x201], [0x202] and
 * [0x207] are set to 3, 3, 1, 1 and both UI flags are cleared; an aborted wait
 * leaves [0x201] at 4.
 *
 * @par Globals
 * D_8009F92C - profile context indexed by byte offset.
 * D_8009F8BD, D_8009F8BE - hold and interaction flags, cleared on completion.
 *
 * @par Control flow
 * Bounded loop (five iterations) with two exit conditions.
 *
 * @par Calls
 * func_80020B64 - format in progress. func_800207B4 - card result code.
 * func_80091270 - frame tick.
 *
 * @par Callers
 * Confirmed: func_8007FB04 (transfer pump, sub-state 3).
 */
void func_8008204C(void)
{
    u32 i;

    if (func_80020B64() != 0) {
        i = 0;
        do {
            func_80091270(0);
            if (func_800207B4(0) != -1) {
                i = 5;
                break;
            }
            if (func_80020B64() == 0) {
                gSaveProfileImage[0x922] = 3;
                gSaveProfileImage[0x201] = 3;
                gSaveProfileImage[0x202] = 1;
                gSaveProfileImage[0x207] = 1;
                gUiHoldFlag = 0;
                gUiInteractionEnabled = 0;
                break;
            }
            i++;
        } while (i < 5);
        if (i == 5) {
            gSaveProfileImage[0x201] = 4;
        }
    } else {
        gSaveProfileImage[0x922] = 3;
        gSaveProfileImage[0x201] = 3;
        gSaveProfileImage[0x202] = 1;
        gSaveProfileImage[0x207] = 1;
        gUiHoldFlag = 0;
        gUiInteractionEnabled = 0;
    }
}
