// Categories: Game Engine > Options Menu | Platform > Sound > Volume
#include "game/types.h"

extern s32 D_8009F908;
extern s32 D_8009F90C;
extern s32 D_8009F910;
extern s32 gSavedOptionsSoundMode __asm__("D_8009F914");
extern s32 gSnapshotPointer __asm__("D_8009F91C");

extern void func_800451F8(s32 volume);
extern void func_80045238(s32 volume);
extern void func_80045294(s32 volume);
extern void func_800451B4(s32 enable);
extern void gStopSoundVoiceHandle(s32 handle) __asm__("func_80046A34");
extern s32 func_80045288(void);
extern s32 func_8002A58C(void);
extern void func_80024630(s32 arg0);

/**
 * @brief Options-screen exit handler that restores sound settings on cancel and stops the preview voice.
 *
 * Entered through the 4-byte D_8007CA30 stub which loads D_8009F918 into $v0 before falling through. When $v0 is 0 (cancel), it restores music, SFX, voice volumes and sound-mode flag from snapshots D_8009F908..D_8009F914; it always stops preview voice D_8009F91C and shuts down active streaming if SFX volume is 0.
 *
 * @param void No C parameters (D_8007CA30 preloads D_8009F918 into $v0).
 *
 * @par Globals
 * D_8009F908/D_8009F90C/D_8009F910/D_8009F914 - saved volume and mode snapshots read on cancel; D_8009F91C - preview voice handle, stopped and zeroed.
 *
 * @par Control flow
 * Three sequential checks: restore snapshots when $v0 == 0; release D_8009F91C if non-zero; call func_80024630(0) when func_80045288() == 0 and (func_8002A58C() & 0xFF) == 1.
 *
 * @par Calls
 * func_800451F8/func_80045238/func_80045294/func_800451B4 - restore volumes/mode; func_80046A34 - stop preview voice; func_80045288/func_8002A58C/func_80024630 - stop stream when SFX muted.
 *
 * @par Side effects
 * Updates mixer volumes/mode, clears D_8009F91C, stops preview voice, and may tear down audio streaming.
 *
 * @par Callers
 * Confirmed: none direct; func_80077AD4 stores the D_8007CA30 prefix at 0x80077D94 into D_800A3778.unk14. Unknown: which menu framework dispatcher calls the +0x14 slot.
 */
void gExitOptionsRestoreSound(void) __asm__("func_8007CA34");

void gExitOptionsRestoreSound(void)
{
    register s32 flag __asm__("$2");

    if (flag == 0) {
        func_800451F8(D_8009F908);
        func_80045238(D_8009F90C);
        func_80045294(D_8009F910);
        func_800451B4(gSavedOptionsSoundMode);
    }
    if (gSnapshotPointer != 0) {
        gStopSoundVoiceHandle(gSnapshotPointer);
        gSnapshotPointer = 0;
    }
    if (func_80045288() == 0) {
        if ((func_8002A58C() & 0xFF) == 1) {
            func_80024630(0);
        }
    }
}
