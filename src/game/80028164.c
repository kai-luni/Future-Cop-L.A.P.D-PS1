// Categories: Graphics > Menu Rendering | Gameplay > State Transitions
#include "game/types.h"

struct State28164 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
};

struct MenuInfo28164 {
    u8 pad00[0x40];
    s32 unk40;
    s32 unk44;
    s32 unk48;
    s32 unk4C;
};

struct Player28164 {
    u8 pad00[0x48];
    s32 unk48;
    u8 pad4C[0x2D4];
    s32 unk320;
};

struct Game28164 {
    u8 pad00[0xAC];
    struct Player28164 *unkAC;
};

struct MenuData28164 {
    u8 *unk00;
    u8 *unk04;
    u8 *unk08;
    u8 pad0C[0x50];
    u8 *unk5C;
    u8 *unk60;
};

extern s32 *gScratchBlockPtr __asm__("D_8009F68C");
extern struct MenuInfo28164 *gGameSession __asm__("D_8009FA94");
extern struct MenuData28164 *D_8009F2C0;
extern struct Game28164 *gPlayerEntities __asm__("D_8009FA58");
extern struct Game28164 *gPlayerEntity2[4] __asm__("D_8009FA5C");
extern s32 gSinglePlayerEntity __asm__("D_8009F9EC");
extern s32 gPlayerCount __asm__("D_8009FA0C");
extern s32 gScreenState __asm__("D_8009FA00");
extern u32 gFrameTime __asm__("D_8009FAA4");
extern u8 D_8009F300[];
extern u8 D_8009ABF0[];
extern u8 D_8009AC08[];
extern u8 D_8009AC14[];
extern u8 D_8009AC20[];
extern u8 D_8009AC2C[];
extern u8 D_8009AC44[];

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void func_8008CD30(u8 *dest, u8 *src, s32 count);
extern void gTranslatePadButtons(void *arg0, s32 arg1) __asm__("func_800576A8");
extern void func_8001C214(u32 color, s32 x, s32 y, s32 w, s32 h, u8 blend);
extern s32 func_80049340(u8 *text);
extern void func_80049100(u8 *text, s32 x, s32 y, s32 color);
extern void func_80027F80(void *arg0, u8 *text, s32 x, s32 y);

/**
 *  @brief Initializes and draws a two-sided player information screen.
 *
 *  The routine prepares persistent screen state, draws player panels and
 *  labels, and selects a subsequent game state from player fields.
 *  @par Globals
 *  D_8009F68C holds screen state; D_8009FA94 and D_8009F2C0 supply menu data.
 *  D_8009FA58 and D_8009FA5C supply players; D_8009F9EC selects the second.
 *  D_8009FA0C controls extra text, D_8009FAA4 its color, and D_8009FA00
 *  receives state changes; D_8009F300 and D_8009ABF0 through D_8009AC44
 *  provide copied data and labels.
 *  @par Control flow
 *  Initialization runs once; branches select text, players and state 5 or 14.
 *  @par Calls
 *  func_80017EBC allocates state; func_8008CD30 copies menu data.
 *  func_800576A8 prepares players; func_8001C214 draws panels.
 *  func_80049340 measures text; func_80049100 and func_80027F80 draw text.
 *  @par Side effects
 *  Updates screen and menu state, may change the game state, and submits drawing.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80028164(void)
{
    struct Game28164 *game;
    register struct Player28164 *p1 __asm__("$20");
    register struct Player28164 *p2 __asm__("$19");
    register u8 *text __asm__("$16");
    s32 length;
    register s32 flag __asm__("$17");
    register s32 limit __asm__("$21");
    register u8 **slot __asm__("$4");
    u8 **tailSlot;
    register s32 blend __asm__("$18");
    s32 x;

    if (gScratchBlockPtr == 0) {
        gScratchBlockPtr = gAllocMemory(0xC8, 3);
        gScratchBlockPtr[0] = 0;
        gScratchBlockPtr[2] = gGameSession->unk40 == gGameSession->unk44;
        if (gPlayerCount == 1) {
            func_8008CD30((u8 *)D_8009F2C0->unk08 + 6, D_8009F300,
                          gGameSession->unk4C + 1);
            if (gScratchBlockPtr[2] == 0) {
                gGameSession->unk4C++;
            }
        }
    }

    gScratchBlockPtr[1] = 3;

    game = gPlayerEntities;
    p1 = game->unkAC;
    if (gSinglePlayerEntity == 0) {
        p2 = gPlayerEntity2[0]->unkAC;
    } else {
        p2 = 0;
    }

    gTranslatePadButtons((u8 *)p1 + 0x30, 0);
    if (p2 != 0) {
        gTranslatePadButtons((u8 *)p2 + 0x30, 1);
    }

    blend = 1;
    func_8001C214(1, 0x14, 0x3E, 0x258, 0x164, blend);
    func_8001C214(0x6000, 0x14, 0x26, 0x12C, 0x18, blend);
    func_8001C214(0x18, 0x140, 0x26, 0x12C, 0x18, blend);

    flag = *(u8 *)&gScratchBlockPtr[2];
    slot = &D_8009F2C0->unk00;
    if (flag) slot++;
    text = *slot;
    length = func_80049340(text);
    limit = 0x12C;
    x = (limit - length) / 4 + 0xA;
    func_80049100(text, x, 0x14, 8);

    slot = &D_8009F2C0->unk00;
    if (!flag) slot++;
    text = *slot;
    length = func_80049340(text);
    x = (limit - length) / 4 + 0xA0;
    func_80049100(text, x, 0x14, 8);

    if (gPlayerCount == blend) {
        text = D_8009F2C0->unk08;
        length = func_80049340(text);
        func_80049100(text, (0x258 - length) / 4 + 0xA, 0x14,
                       (gFrameTime & 0x20) ? 8 : 2);
    }

    func_80027F80(0, D_8009ABF0, 0xA0, 9);
    func_80027F80(0, D_8009AC08, 0x28, 2);
    func_80027F80(0, D_8009AC08, 0x118, 2);
    func_80027F80(0, D_8009AC14, 0x64, 2);
    func_80027F80(0, D_8009AC14, 0xDC, 2);
    func_80027F80(p1, D_8009AC20, 0x28, 8);
    func_80027F80(p1, D_8009AC2C, 0x46, 8);
    func_80027F80(p1, D_8009AC44, 0x64, 8);
    func_80027F80(p2, D_8009AC20, 0x118, 8);
    func_80027F80(p2, D_8009AC2C, 0xFA, 8);
    func_80027F80(p2, D_8009AC44, 0xDC, 8);

    func_8001C214(0x300, 0x14, 0x1A2, 0x258, 0x18, 1);

    if (gScratchBlockPtr[2] == 0 && gPlayerCount == 1) {
        tailSlot = &D_8009F2C0->unk60;
    } else {
        tailSlot = &D_8009F2C0->unk5C;
    }
    text = *tailSlot;
    length = func_80049340(text);
    func_80049100(text, (0x258 - length) / 4 + 0xA, 0xD2, 8);

    if (p1->unk48 != 0 && (p2 == 0 || p2->unk48 != 0)) {
        if (p1->unk320 == 0) goto state14;
        if (p2 == 0) goto done;
        if (p2->unk320 != 0) goto done;
    }
    if (p1->unk320 == 0) goto state14;
    if (p2 == 0 || p2->unk320 != 0) {
        gScreenState = 5;
        goto done;
    }
state14:
    gScreenState = 0xE;
done:

    gScratchBlockPtr[0] = 1;
}
