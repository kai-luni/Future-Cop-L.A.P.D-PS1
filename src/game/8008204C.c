// Categories: Resources > Movie Playback | Platform > CD-ROM
#include "game/types.h"

extern u8 *D_8009F92C;
extern u8 D_8009F8BD;
extern u8 D_8009F8BE;
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern u8 D_8009F8C0[];

extern s32 func_80020B64(void);
extern void func_80091270(s32 arg0);
extern s32 func_800207B4(s32 arg0);
extern s32 func_80078740(void);

/**
 * @brief Waits for a streaming transfer to finish and sets the video registers.
 *
 * While func_80020B64 reports an active transfer the loop waits one tick
 * (func_80091270(0)) and polls func_800207B4(0). Any answer other than -1
 * aborts after five iterations; if the transfer ends on its own the state
 * bytes at D_8009F92C[0x922], [0x201], [0x202] and [0x207] are set to
 * 3, 3, 1 and 1 and both D_8009F8BD and D_8009F8BE are cleared. The same block
 * is written when no transfer is running at all; an aborted wait leaves
 * [0x201] at 4.
 *
 * @par Globals
 * D_8009F92C - video/decoder state block indexed by byte offset.
 * D_8009F8BD, D_8009F8BE - per-frame flags cleared on completion.
 *
 * @par Control flow
 * Bounded loop (five iterations) with two exit conditions.
 *
 * @par Calls
 * func_80020B64 - transfer active. func_800207B4 - transfer poll.
 * func_80091270 - frame tick.
 *
 * @par Side effects
 * Writes four state bytes plus the two flags.
 *
 * @par Callers
 * Confirmed: func_8007FB04 (film player).
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
                D_8009F92C[0x922] = 3;
                D_8009F92C[0x201] = 3;
                D_8009F92C[0x202] = 1;
                D_8009F92C[0x207] = 1;
                D_8009F8BD = 0;
                D_8009F8BE = 0;
                break;
            }
            i++;
        } while (i < 5);
        if (i == 5) {
            D_8009F92C[0x201] = 4;
        }
    } else {
        D_8009F92C[0x922] = 3;
        D_8009F92C[0x201] = 3;
        D_8009F92C[0x202] = 1;
        D_8009F92C[0x207] = 1;
        D_8009F8BD = 0;
        D_8009F8BE = 0;
    }
}
