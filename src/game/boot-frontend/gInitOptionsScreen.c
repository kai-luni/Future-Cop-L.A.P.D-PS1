// Categories: Game Engine > Options Menu | Platform > Sound > Volume
#include "game/types.h"

typedef struct {
    u8 pad00[0xC];
    s32 unk0C;
    u8 pad10[0x4C];
    s32 unk5C;
} Unk8009FA94;

extern u8 gSnapshotResult __asm__("D_8009F904");
extern s32 gSavedOptionsSfxVolume __asm__("D_8009F908");
extern s32 gSavedOptionsVagmVolume __asm__("D_8009F90C");
extern s32 gSavedOptionsVoiceVolume __asm__("D_8009F910");
extern volatile s32 gSavedOptionsSoundMode __asm__("D_8009F914");
extern s32 gTestResultFlag __asm__("D_8009F918");
extern volatile s32 gSnapshotPointer __asm__("D_8009F91C");
extern volatile s32 gPreviewTrackId __asm__("D_8009F920");
extern Unk8009FA94 *gGameSession[2] __asm__("D_8009FA94");
extern u16 gSoundWidgetLabel[] __asm__("D_8009BABE");
extern u8 gAudioOptionsMenuBackgroundIndex[] __asm__("D_800A377B");
extern void gStartVagmAudioStream(s32) __asm__("func_8002A16C");
extern s32 gIsVagmAudioStreamActive(void) __asm__("func_8002A58C");
extern s32 gGetStereoSoundMode(void) __asm__("func_800451E0");
extern s32 gGetMusicVolume(void) __asm__("func_8004522C");
extern s32 gGetVagmVolumeSetting(void) __asm__("func_80045288");
extern s32 gGetVoiceVolume(void) __asm__("func_800452C8");

/**
 * @brief Options-screen init callback: snapshots volumes and the sound-mode flag, primes the preview-sound state and picks the initial menu flag byte.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F908/90C/910 - snapshots of the three volume getters; D_8009F914 - snapshot of the sound-mode flag D_8009F730; D_8009F918 - exit request, cleared; D_8009F904 - selection byte, cleared.
 * D_8009F91C/D_8009F920 - preview voice handle (0) and preview id word (0x47). D_8009BABE[0] - toggle label text id 0x100E (flag on) or 0x1018 (off). D_800A377B[0] - menu flag byte: bit 0 from state block +0xC == 1, bit 3 from +0x5C == 1 (values 1/3/8/9).
 *
 * @par Control flow
 * Straight-line; the stream-idle test gates func_8002A16C(0), and a 2x2 branch on the two state-block words (read through the D_8009FA94 pointer via inline asm) selects the flag byte.
 *
 * @par Calls
 * func_8004522C/func_80045288/func_800452C8 - music/SE/voice volume getters. func_800451E0 - sound-mode flag getter. func_8002A58C - stream busy test (D_8009F6DC != 0). func_8002A16C - starts a stream/CD audio request when idle.
 *
 * @par Side effects
 * Writes the options state words, the toggle text id and the menu flag byte; may start audio streaming.
 *
 * @par Callers
 * Confirmed: no direct callers; the address is stored at 0x80077D70 (inside func_80077AD4) into D_800A3778+0x10 as the init callback beside update func_8007CACC and draw func_8007CFA0. Unknown: which framework routine performs the indirect call.
 */
void gInitOptionsScreen(void) __asm__("func_8007D18C");

void gInitOptionsScreen(void)
{
    register Unk8009FA94 *p __asm__("$3");
    s32 tmp;

    gTestResultFlag = 0;
    gSavedOptionsSfxVolume = gGetMusicVolume();
    gSavedOptionsVagmVolume = gGetVagmVolumeSetting();
    gSavedOptionsVoiceVolume = gGetVoiceVolume();
    tmp = gGetStereoSoundMode();
    gSavedOptionsSoundMode = tmp;
    gSnapshotPointer = 0;
    gPreviewTrackId = 0x47;
    gSoundWidgetLabel[0] = (tmp != 0) ? 0x100E : 0x1018;
    gSnapshotResult = 0;
    if ((u8)gIsVagmAudioStreamActive() == 0) {
        gStartVagmAudioStream(0);
    }
    __asm__ volatile("lui %0,%%hi(D_8009FA94)\n\tlw %0,%%lo(D_8009FA94)(%0)" : "=r" (p) : : "memory");
    if (p->unk5C == 1) {
        if (p->unk0C == 1) {
            gAudioOptionsMenuBackgroundIndex[0] = 9;
        } else {
            gAudioOptionsMenuBackgroundIndex[0] = 3;
        }
    } else if (p->unk0C == 1) {
        gAudioOptionsMenuBackgroundIndex[0] = 1;
    } else {
        gAudioOptionsMenuBackgroundIndex[0] = 8;
    }
}
