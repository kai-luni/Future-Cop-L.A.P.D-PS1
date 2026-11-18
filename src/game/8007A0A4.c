// Categories: Graphics > Text > Debug Numbers
#include "game/types.h"

struct UnkB4E4 {
    u8 unk00[0x08];
    s32 unk08;
    u8 unk0C[0x18];
};

extern struct UnkB4E4 gWideTextTable[] __asm__("D_8009B4E4");
extern s8 gFrontEndListTable[] __asm__("D_8009B72C");
extern u8 gMovieAbortFlag __asm__("D_8009F4E0");
extern u8 D_80011E60[];

extern void func_80082DDC(s32 arg0, s32 arg1);

/**
 * @brief Plays the videos referenced by the five-entry front-end list.
 *
 * @param arg0 Only action 3 starts playback.
 * @return Nothing.
 *
 * @par Confirmed
 * The loop matches func_80079FE0 but uses five entries of D_8009B72C.
 * Non-zero values go to func_80082DDC(val, 2), gMovieAbortFlag aborts,
 * and D_80011E60 names the credits movie played afterward if not aborted.
 *
 * @par Callers
 * The retail main-menu item table D_8009BBF8 references this handler at
 * entry 5; func_8007A168 can enable that entry.
 */
void func_8007A0A4(s32 arg0)
{
    u32 i;
    s32 val;

    if (arg0 == 3) {
        i = 0;
        do {
            val = gWideTextTable[gFrontEndListTable[i]].unk08;
            if (val != 0) {
                func_80082DDC(val, 2);
                if (gMovieAbortFlag != 0) {
                    return;
                }
            }
            i++;
        } while (i < 5);
        if (gMovieAbortFlag == 0) {
            func_80082DDC((s32)D_80011E60, 0);
        }
    }
}
