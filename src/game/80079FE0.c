// Categories: Graphics > Text > Debug Numbers
#include "game/types.h"

struct UnkB4E4 {
    u8 unk00[0x08];
    s32 unk08;
    u8 unk0C[0x18];
};

extern struct UnkB4E4 gWideTextTable[] __asm__("D_8009B4E4");
extern s8 gMissionListTable[] __asm__("D_8009B724");
extern u8 gMovieAbortFlag __asm__("D_8009F4E0");
extern u8 gCreditsMovieFilename[] __asm__("D_80011E60");

extern void gPlayMovieClip(s32 arg0, s32 arg1) __asm__("func_80082DDC");

/**
 * @brief Plays the videos referenced by the eight-entry mission list.
 *
 * @param arg0 Only action 3 starts playback.
 * @return Nothing.
 *
 * @par Confirmed
 * Walks i = 0..7 over D_8009B724; each non-zero D_8009B4E4[table[i]].unk08
 * is passed to func_80082DDC(val, 2) for playback. gMovieAbortFlag aborts
 * the loop early; otherwise the credits movie D_80011E60 is played.
 *
 * @par Unknown
 * The exact meaning of gMovieAbortFlag.
 *
 * @par Callers
 * The retail main-menu item table D_8009BBF8 references this handler at
 * entry 2; func_8007A168 can enable that entry.
 */
void gPlayCrimeWarMissionMovies(s32 arg0) __asm__("func_80079FE0");

void gPlayCrimeWarMissionMovies(s32 arg0)
{
    u32 i;
    s32 val;

    if (arg0 == 3) {
        i = 0;
        do {
            val = gWideTextTable[gMissionListTable[i]].unk08;
            if (val != 0) {
                gPlayMovieClip(val, 2);
                if (gMovieAbortFlag != 0) {
                    return;
                }
            }
            i++;
        } while (i < 8);
        if (gMovieAbortFlag == 0) {
            gPlayMovieClip((s32)gCreditsMovieFilename, 0);
        }
    }
}