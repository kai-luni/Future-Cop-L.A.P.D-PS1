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
 * func_80020B64 queues a format when the card manager is idle and returns 1
 * when it is busy. While busy, this function waits one tick and retries up to
 * five times, stopping early if func_800207B4(0) returns anything other than -1.
 * Once the format is queued, bytes [0x922], [0x201], [0x202] and [0x207] are
 * set to 3, 3, 1, 1 and both UI flags are cleared. Exhaustion or an early stop
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
 * func_80020B64 - queues a format if idle. func_800207B4 - card result code.
 * func_80091270 - frame tick.
 *
 * @par Callers
 * Confirmed: func_8007FB04 (transfer pump, sub-state 3).
 */
void gQueueMemoryCardFormat(void) __asm__("func_8008204C");

void gQueueMemoryCardFormat(void)
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
