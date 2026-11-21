// Categories: Game Engine > Front-End Configuration | Resources > Slot Table
#include "game/types.h"

struct UnkA3808 {
    u8 unk00[0x24];
    u32 unk24;
};

struct UnkB4E4 {
    u8 unk00[0x1F];
    u8 unk1F;
    u8 unk20;
    u8 pad[0x03];
};

extern struct UnkA3808 gMenuDescriptor __asm__("D_800A3808");
extern u8 gPauseFlag __asm__("D_8009F3E8");
extern u8 gRowClearFlag __asm__("D_8009FA10");
extern u8 gCreditsMovieFilename[] __asm__("D_80011E60");
extern struct UnkB4E4 gWideTextTable[] __asm__("D_8009B4E4");

extern void func_80082DDC(s32 arg0, s32 arg1);
extern void gUpdateMainMenuUnlocks(void) __asm__("func_8007A168");

/**
 * @brief Resets the profile list state: clears the pending flag, the 16
 *        slot markers and re-runs the list init.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * When D_800A3808.unk24 is set: with D_8009F3E8 non-zero the credits movie
 * is played via func_80082DDC(D_80011E60, 0) and the flag cleared, then unk24
 * is zeroed. D_8009FA10 becomes 0, the +0x1F/+0x20 bytes of all 16 entries
 * in D_8009B4E4 are cleared, and func_8007A168 runs.
 *
 * @par Likely role
 * The screen-enter reset of the profile list (registered by func_80077AD4
 * in D_800A3808.unk10): the two per-slot bytes are the transient selection
 * markers the list drawer func_8007B460 reads.
 *
 * @par Unknown
 * The precise interpretation of the remaining per-slot state.
 *
 * @par Callers
 * No direct callers in the call graph. func_80077AD4 stores the address in
 * the screen callback block D_800A3808.unk10, so the screen dispatcher
 * reaches it indirectly.
 */
void gResetProfileListState(void) __asm__("func_80079F54");

void gResetProfileListState(void)
{
    u32 i;

    if (gMenuDescriptor.unk24 != 0) {
        if (gPauseFlag != 0) {
            func_80082DDC((s32)gCreditsMovieFilename, 0);
            gPauseFlag = 0;
        }
        gMenuDescriptor.unk24 = 0;
    }
    gRowClearFlag = 0;
    for (i = 0; i < 16; i++) {
        gWideTextTable[i].unk1F = 0;
        gWideTextTable[i].unk20 = 0;
    }
    gUpdateMainMenuUnlocks();
}
