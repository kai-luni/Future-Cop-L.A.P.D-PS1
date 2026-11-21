// Categories: Game Engine > State
#include "game/types.h"

typedef struct {
    u8 pad00[8];
    u16 unk08;
    u8 pad0A[2];
    u8 unk0C;
    u8 pad0D[3];
    u16 unk10;
    u8 pad12[2];
    u8 unk14;
    u8 pad15[3];
    u16 unk18;
    u8 pad1A[2];
    u8 unk1C;
    u8 pad1D[3];
    u16 unk20;
    u8 pad22[2];
    u8 unk24;
} Unk8007CE04;

typedef struct {
    u8 pad00[0xC];
    s32 unk0C;
    u8 pad10[0x4C];
    s32 unk5C;
} Unk8009FA94;

extern s8 *gFontBankTable __asm__("D_8009F468");
extern u8 gSnapshotResult __asm__("D_8009F904");
extern s32 D_8009F908;
extern s32 D_8009F90C;
extern s32 D_8009F910;
extern volatile s32 gSavedOptionsSoundMode __asm__("D_8009F914");
extern s32 gTestResultFlag __asm__("D_8009F918");
extern volatile s32 gSnapshotPointer __asm__("D_8009F91C");
extern volatile s32 gPreviewTrackId __asm__("D_8009F920");

extern s32 gGameClock[] __asm__("D_8009FA60");
extern Unk8009FA94 *gGameSession[2] __asm__("D_8009FA94");
extern u16 gSoundWidgetLabel[] __asm__("D_8009BABE");
extern u8 D_800A377B[];

extern void gEmitFlatQuadRectMode(s32, s32, s32, s32, s32, s32, s32, s32, s32) __asm__("func_80082FA4");
extern Unk8007CE04 *gEmitAtlasSpriteSimple(s32, s32, s32, s32, s32, s32, s32, s32, s32) __asm__("func_80082AB0");
extern void gDrawAlignedMultilineText(s32, s32, s32, s32, s32, s32, s32, s32) __asm__("func_800788F0");
extern void gQueueStatusLineText(s32, s32, s32) __asm__("func_80078B34");
extern void gStartVagmAudioStream(s32) __asm__("func_8002A16C");
extern s32 gIsVagmAudioStreamActive(void) __asm__("func_8002A58C");
extern void gSetStereoSoundMode(s32 enable) __asm__("func_800451B4");
extern s32 gGetStereoSoundMode(void) __asm__("func_800451E0");
extern s32 gGetMusicVolume(void) __asm__("func_8004522C");
extern s32 gGetVagmVolumeSetting(void) __asm__("func_80045288");
extern s32 gGetVoiceVolume(void) __asm__("func_800452C8");
extern void gStopSoundVoiceHandle(s32 handle) __asm__("func_80046A34");
extern s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1) __asm__("func_80046C84");
void gConfirmSoundOptionsItem(s32 arg0) __asm__("D_8007D138");


/**
 *  @brief Sets a global flag when the argument equals 3.
 *
 *  This entry point conditionally updates a shared state value.
 *  The source does not establish the meaning of the value or argument.
 *  @param arg0 Value tested against 3.
 *  @par Globals
 *  D_8009F918 is set to 1 when the test succeeds.
 *  @par Control flow
 *  Branches on arg0; other values leave the global unchanged.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May modify D_8009F918.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 */
void gConfirmSoundOptionsItem(s32 arg0)
{
    if (arg0 == 3) {
        gTestResultFlag = 1;
    }
}
