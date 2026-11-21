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
extern struct MenuData28164 *gResultsTextTable __asm__("D_8009F2C0");
extern struct Game28164 *gPlayerEntities __asm__("D_8009FA58");
extern struct Game28164 *gPlayerEntity2[4] __asm__("D_8009FA5C");
extern s32 gSinglePlayerEntity __asm__("D_8009F9EC");
extern s32 gPlayerCount __asm__("D_8009FA0C");
extern s32 gScreenState __asm__("D_8009FA00");
extern u32 gFrameTime __asm__("D_8009FAA4");
extern u8 gResultsRoundFormat[] __asm__("D_8009F300");
extern u8 gResultsCenterLabelCodes[] __asm__("D_8009ABF0");
extern u8 gResultsOuterLabelCodes[] __asm__("D_8009AC08");
extern u8 gResultsInnerLabelCodes[] __asm__("D_8009AC14");
extern u8 gResultsOuterStatCodes[] __asm__("D_8009AC20");
extern u8 gResultsMiddleStatCodes[] __asm__("D_8009AC2C");
extern u8 gResultsInnerStatCodes[] __asm__("D_8009AC44");

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void gSpillVarargsArguments(u8 *dest, u8 *src, s32 count) __asm__("func_8008CD30");
extern void gTranslatePadButtons(void *arg0, s32 arg1) __asm__("func_800576A8");
extern void gEmitTranslucentTileRect(u32 color, s32 x, s32 y, s32 w, s32 h, u8 blend) __asm__("func_8001C214");
extern s32 gMeasureTextWidthPx(u8 *text) __asm__("func_80049340");
extern void gDrawAlignedString(u8 *text, s32 x, s32 y, s32 color) __asm__("func_80049100");
extern void gDrawResultsTextBlock(void *arg0, u8 *text, s32 x, s32 y) __asm__("func_80027F80");

void gDrawPlayerResultsScreen(void) __asm__("func_80028164");

/**
 * @brief Initializes and draws the player results screen, then checks for a screen transition.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Allocates a 0xC8-byte cache on first use, records the session comparison flag and formats the
 * solo counter when applicable. Resets the cache index, prepares player data, draws the panels
 * and issues eleven control-coded text calls. Checks the two players' +0x48 and +0x320 words to
 * determine whether to write state 5 or 0x0E, then marks the stat cache populated.
 *
 * @par Globals
 * Reads gGameSession, gPlayerCount, gSinglePlayerEntity, gPlayerEntities, gPlayerEntity2 and
 * gFrameTime. Uses gResultsTextTable, gResultsRoundFormat and the six results text code arrays.
 * Updates gScratchBlockPtr, the solo label and potentially gScreenState.
 *
 * @par Callers
 * Direct calls from gRunGameFrameLoop (func_80028618).
 *
 * @par Role and path
 * The gameplay frame loop draws this persistent results view until its transition conditions
 * are met.
 *
 * @par Unknown
 * The player fields at +0x48 and +0x320 determine the exact transition conditions, and the
 * screen meanings are defined by the dispatcher.
 */
void gDrawPlayerResultsScreen(void)
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
            gSpillVarargsArguments((u8 *)gResultsTextTable->unk08 + 6, gResultsRoundFormat,
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
    gEmitTranslucentTileRect(1, 0x14, 0x3E, 0x258, 0x164, blend);
    gEmitTranslucentTileRect(0x6000, 0x14, 0x26, 0x12C, 0x18, blend);
    gEmitTranslucentTileRect(0x18, 0x140, 0x26, 0x12C, 0x18, blend);

    flag = *(u8 *)&gScratchBlockPtr[2];
    slot = &gResultsTextTable->unk00;
    if (flag) slot++;
    text = *slot;
    length = gMeasureTextWidthPx(text);
    limit = 0x12C;
    x = (limit - length) / 4 + 0xA;
    gDrawAlignedString(text, x, 0x14, 8);

    slot = &gResultsTextTable->unk00;
    if (!flag) slot++;
    text = *slot;
    length = gMeasureTextWidthPx(text);
    x = (limit - length) / 4 + 0xA0;
    gDrawAlignedString(text, x, 0x14, 8);

    if (gPlayerCount == blend) {
        text = gResultsTextTable->unk08;
        length = gMeasureTextWidthPx(text);
        gDrawAlignedString(text, (0x258 - length) / 4 + 0xA, 0x14,
                       (gFrameTime & 0x20) ? 8 : 2);
    }

    gDrawResultsTextBlock(0, gResultsCenterLabelCodes, 0xA0, 9);
    gDrawResultsTextBlock(0, gResultsOuterLabelCodes, 0x28, 2);
    gDrawResultsTextBlock(0, gResultsOuterLabelCodes, 0x118, 2);
    gDrawResultsTextBlock(0, gResultsInnerLabelCodes, 0x64, 2);
    gDrawResultsTextBlock(0, gResultsInnerLabelCodes, 0xDC, 2);
    gDrawResultsTextBlock(p1, gResultsOuterStatCodes, 0x28, 8);
    gDrawResultsTextBlock(p1, gResultsMiddleStatCodes, 0x46, 8);
    gDrawResultsTextBlock(p1, gResultsInnerStatCodes, 0x64, 8);
    gDrawResultsTextBlock(p2, gResultsOuterStatCodes, 0x118, 8);
    gDrawResultsTextBlock(p2, gResultsMiddleStatCodes, 0xFA, 8);
    gDrawResultsTextBlock(p2, gResultsInnerStatCodes, 0xDC, 8);

    gEmitTranslucentTileRect(0x300, 0x14, 0x1A2, 0x258, 0x18, 1);

    if (gScratchBlockPtr[2] == 0 && gPlayerCount == 1) {
        tailSlot = &gResultsTextTable->unk60;
    } else {
        tailSlot = &gResultsTextTable->unk5C;
    }
    text = *tailSlot;
    length = gMeasureTextWidthPx(text);
    gDrawAlignedString(text, (0x258 - length) / 4 + 0xA, 0xD2, 8);

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
