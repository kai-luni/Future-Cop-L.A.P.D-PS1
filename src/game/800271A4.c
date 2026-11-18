// Categories: Gameplay > Frame Update | Input > Actions | Graphics > Text
#include "game/types.h"

struct Vec3 { s32 x; s32 y; s32 z; };

struct InputAction {
    u32 buttons;
    s32 phase;
};

struct Player {
    u8 pad00[0x30];
    u32 unk30;
    u8 pad34[0x2D0];
    struct Vec3 saved_position;
    s8 unk310;
    u8 pad311[1];
    s8 unk312;
    u8 pad313[1];
    s16 unk314;
    s16 unk316;
    u8 pad318[4];
    struct InputAction unk31C;
};

struct Game {
    u8 pad00[0x24];
    u32 unk24;
    u8 pad28[0x28];
    struct Vec3 position;
    u8 pad5C[0x50];
    struct Player *unkAC;
};

struct Unk9A0 {
    u8 pad00[0x11F];
    u8 unk11F;
};

extern struct Game *D_8009FA58;
extern s32 gSlotVoiceHandle __asm__("D_8009F678");
extern u32 gScreenState __asm__("D_8009FA00");
extern u32 gSceneFrameCounter __asm__("D_8009FA50");
extern s32 gModuleLoadedFlag __asm__("D_8009FA84");
extern u8 *D_8009F2D0;
extern u32 gFrameTickCounter __asm__("D_8009FA2C");
extern u8 gMenuModeFlag __asm__("D_8009F680");
extern s32 D_8009FA38;
extern s32 gPaletteCycleCounter __asm__("D_8009FA60");
extern u32 gDiscShadeByte __asm__("D_8009FAA4");
extern s32 gFrameTickBase __asm__("D_8009FA30");
extern s32 D_8009FA24;
extern s32 gFrameId __asm__("D_8009F9FC");
extern u8 *gMissionWorkingSet[4] __asm__("D_8009F9A0");
extern u8 gBannerString[] __asm__("D_8009F2F4");

extern void func_8005780C(u32 buttons, struct InputAction *action);
extern s32 func_80046C84(s32 id, s32 arg1);
extern void func_80046A34(s32 handle);
extern s32 func_80049340(u8 *text);
extern void func_80049100(u8 *text, s32 x, s32 y, s32 color);
extern void func_80047690(u8 enable);
extern void func_80069D0C(s32 amount);
extern void func_8004CD6C(s32 message, s32 arg2, s32 arg3);
extern void func_8004CE04(void);
extern void func_8001BBAC(void);
extern void func_8001EB6C(void);
extern void gSoundPauseVoices(void) __asm__("func_800476D0");
extern void gSweepRetiredEntities(void) __asm__("func_8004D598");
extern s32 func_8001EF34(s32 arg0);
extern void gUpdateLightDirection(void) __asm__("func_80013C80");
extern void func_80026D1C(void);
extern void func_80026D5C(void);
extern void func_80026D9C(s32 arg0);
extern void func_80026DDC(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
extern void func_80026E1C(s32 arg0, s32 arg1);
extern void func_80026E5C(s32 arg0, s32 arg1);
extern void func_80026F68(s32 arg0, s32 arg1);
extern void func_80031DE8(s32 arg0);
extern void func_8003F570(s32 arg0);
extern void func_80047F30(s32 arg0);
extern void func_8004CECC(void);
extern void func_8004DF28(void);
extern void func_800616A0(s32 arg0);

/**
 *  @brief Updates player action state and dispatches gameplay or paused-frame work.
 *
 *  Processes the player's action, toggles a player state under specific conditions,
 *  and uses local flags to select update paths and display status text.
 *  @par Globals
 *  D_8009FA58 supplies game and player state; D_8009FA00, D_8009FA50,
 *  D_8009FA84, D_8009FA2C, and D_8009F9A0 gate actions and display.
 *  D_8009F2D0 and D_8009F2F4 supply text; D_8009F678 holds a handle.
 *  D_8009F680, D_8009FA38, D_8009FA60, D_8009FAA4, D_8009FA30,
 *  D_8009FA24, and D_8009F9FC track pause, timing, and update state.
 *  @par Control flow
 *  Branches on action phase, player state, and display conditions; pause flags
 *  suppress selected updates and trigger transition work on entry or exit.
 *  @par Calls
 *  func_8005780C processes input; func_80049340 and func_80049100 measure and
 *  draw text. The func_80026DDC or func_80026E5C path dispatches player work;
 *  other calls update subsystems, timing, and pause transitions.
 *  @par Side effects
 *  Changes player and global state, draws text, and invokes subsystem updates.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_800271A4(void)
{
    struct Player *player;
    struct Unk9A0 *unk;
    u32 flags;
    u8 *text;
    s32 x;
    s32 color;
    s32 clock_value;

    player = D_8009FA58->unkAC;
    func_8005780C(player->unk30, &player->unk31C);

    flags = 0;
    if (player->unk31C.phase == 0 && gScreenState == 4 && player->unk312 == -1) {
        func_80046C84(0x3E, (s32)D_8009FA58);
        *(volatile s16 *)&player->unk316 = 0;
        *(volatile s16 *)&player->unk314 = 0;
        player->saved_position = D_8009FA58->position;
        player->unk310 = !player->unk310;
        if (!player->unk310) {
            if (gSlotVoiceHandle != 0) {
                func_80046A34(gSlotVoiceHandle);
                gSlotVoiceHandle = 0;
            }
        }
    }

    if (!(D_8009FA58->unk24 & 0x1000000) && player->unk310 != 0) {
        flags |= 0x21;
    }

    if (gSceneFrameCounter >= 2 && func_8001EF34(0) == 0 && gModuleLoadedFlag == 0) {
        text = D_8009F2D0;
        x = (0x280 - func_80049340(text)) / 4;
        color = 1;
        if (gFrameTickCounter & 0x80) {
            color = 9;
        }
        func_80049100(text, x, 0xB4, color);
        flags |= 0x20;
    }

    if (!(flags & 0x20)) {
        if (gMenuModeFlag != 0) {
            gPaletteCycleCounter = D_8009FA38;
            clock_value = *(volatile s32 *)&gPaletteCycleCounter;
            gDiscShadeByte = clock_value;
            gFrameTickBase = clock_value;
            func_80047690(1);
        }
        gMenuModeFlag = 0;
        func_80069D0C(gFrameId);
        func_8004DF28();
        func_8004CECC();
        func_8004CD6C(6, gFrameId, 0);
        func_80031DE8(gFrameId);
        func_8004CE04();
        func_8003F570(gFrameId);
    }

    if (flags & 1) {
        func_80026DDC((s32)D_8009FA58, 0, flags & 0x20, 0);
    } else {
        gUpdateLightDirection();
        func_80026E5C(0, (s32)D_8009FA58);
        if (!(flags & 0x20)) {
            func_80026D5C();
        }
        func_80026E1C(0, (s32)D_8009FA58);
        func_80026F68(0, 1);
        unk = (struct Unk9A0 *)gMissionWorkingSet[0];
        if (unk == 0 || unk->unk11F == 0) {
            func_80047F30((s32)D_8009FA58);
        }
    }

    if (gModuleLoadedFlag > 0 && (gDiscShadeByte & 0x100)) {
        func_80049100(gBannerString, 0x91, 0x14, 1);
    }

    func_8001BBAC();

    if (!(flags & 0x20)) {
        func_80026D1C();
        func_800616A0(gFrameId);
        func_80026D9C(gFrameId);
        gSweepRetiredEntities();
        D_8009FA24 = 0;
    } else {
        if (gMenuModeFlag == 0) {
            D_8009FA38 = gPaletteCycleCounter;
            func_8001EB6C();
            gSoundPauseVoices();
            func_80046C84(0x3E, (s32)D_8009FA58);
        }
        gMenuModeFlag = 1;
        D_8009FA24 = 1;
    }
}
