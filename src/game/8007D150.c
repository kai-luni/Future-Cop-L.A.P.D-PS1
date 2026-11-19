// Categories: Game Engine > Options Menu | Platform > Sound > Voices
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
extern volatile s32 D_8009F914;
extern s32 gTestResultFlag __asm__("D_8009F918");
extern volatile s32 gSnapshotPointer __asm__("D_8009F91C");
extern volatile s32 gPreviewTrackId __asm__("D_8009F920");

extern s32 gGameClock[] __asm__("D_8009FA60");
extern Unk8009FA94 *gGameSession[2] __asm__("D_8009FA94");
extern u16 gSoundWidgetLabel[] __asm__("D_8009BABE");
extern u8 D_800A377B[];

extern void gEmitFlatQuadRectMode(s32, s32, s32, s32, s32, s32, s32, s32, s32) __asm__("func_80082FA4");
extern Unk8007CE04 *gEmitAtlasSpriteSimple(s32, s32, s32, s32, s32, s32, s32, s32, s32) __asm__("func_80082AB0");
extern void func_800788F0(s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80078B34(s32, s32, s32);
extern void func_8002A16C(s32);
extern s32 func_8002A58C(void);
extern void func_800451B4(s32 enable);
extern s32 func_800451E0(void);
extern s32 func_8004522C(void);
extern s32 func_80045288(void);
extern s32 func_800452C8(void);
extern void func_80046A34(s32 handle);
extern s32 func_80046C84(s32 id, s32 arg1);

/**
 * @brief Options-menu widget callback that stops the preview voice when action 3 is received.
 *
 * Checks action code arg0 == 3; if the options preview sound voice handle D_8009F91C is non-zero,
 * releases the voice through func_80046A34 and resets D_8009F91C to 0.
 *
 * @param arg0 Menu widget action code (only action 3 triggers preview voice teardown).
 *
 * @par Globals
 * D_8009F91C - active preview sound voice handle (read, released, and cleared to 0).
 *
 * @par Control flow
 * Guarded by arg0 == 3 and D_8009F91C != 0; otherwise returns immediately.
 *
 * @par Calls
 * func_80046A34 - stops and frees the voice handle held in D_8009F91C.
 *
 * @par Side effects
 * Stops the options preview voice and zeroes D_8009F91C.
 *
 * @par Callers
 * Confirmed: none direct; address 0x8007D150 is stored in options widget table D_8009BABE at 0x8009BAC8 / Likely: dispatched indirectly by options menu UI on widget exit/cancel (action 3) / Unknown: exact table dispatcher.
 */
void gStopOptionsPreview(s32 arg0) __asm__("func_8007D150");

void gStopOptionsPreview(s32 arg0)
{
    s32 handle;

    if (arg0 == 3) {
        handle = gSnapshotPointer;
        if (handle != 0) {
            func_80046A34(handle);
            gSnapshotPointer = 0;
        }
    }
}
