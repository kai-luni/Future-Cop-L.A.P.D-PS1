// Categories: Game Engine > Menu | Input > Controller | Graphics > Menu Rendering
#include "game/types.h"

struct InputAction { u32 buttons; s32 phase; };
struct Item {
    void *text;
    void (*handler)(s32, s32, s32, s32, s32);
    struct Menu *next;
    s16 icon;
    u8 enabled;
    s8 player;
    u16 x, y;
    u8 flags, unk15;
    u16 unk16;
    s8 color;
    u8 pad19[3];
};
struct Menu {
    s8 selection[2];
    s8 item_count;
    u8 pad03[2];
    s8 mode;
    s16 width;
    u8 pad08;
    s8 extra;
    u8 x, y;
    u32 (*filter)(u32, s32);
    u8 pad10[8];
    void (*update)(s32);
    u8 pad1c[4];
    void (*cancel)(void);
    u32 unk24;
    struct Item *items;
};
extern s32 gStillRunningState __asm__("D_8009F464");
extern struct Menu *gFontBankTable __asm__("D_8009F468");
extern u8 gMenuSetupCalled __asm__("D_8009F8CC");
extern u8 gAttractDemoSequenceIndex __asm__("D_8009F3F2");
extern u8 gAttractMovieIndex __asm__("D_8009F3F3");
extern u8 gFrontEndReturnMode __asm__("D_8009F45A");
extern u8 gUiInteractionEnabled __asm__("D_8009F8BE");
extern s32 gPlayer1PrevMenuButtons __asm__("D_8009F8B0");
extern s32 gPlayer2PrevMenuButtons __asm__("D_8009F8B4");
extern s32 gIdleDeadline __asm__("D_8009F870");
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern s32 prev_buttons[2] __asm__("D_8009F8B0");
extern u32 gPlayer1MenuButtons __asm__("D_8009F8D0");
extern u32 gPlayer2MenuButtons __asm__("D_8009F8D4");
extern volatile u32 D_8009F8D0_arr[] __asm__("D_8009F8D0");
extern s32 gMenuUpHoldTicks[] __asm__("D_8009F890");
extern s32 gMenuDownHoldTicks[] __asm__("D_8009F898");
extern s32 gMenuLeftHoldTicks[] __asm__("D_8009F8A0");
extern s32 gMenuRightHoldTicks[] __asm__("D_8009F8A8");
extern volatile s32 gFrameTickCounter __asm__("D_8009FA2C");
extern u32 gAttractMovieNames[] __asm__("D_8009B324");
extern struct Menu gMenuDescriptor __asm__("D_800A3808");
extern s16 gSlotRecordTable[2][4] __asm__("D_800A3578");
extern struct InputAction gMenuUpActions[] __asm__("D_800A3588");
extern struct InputAction gMenuDownActions[] __asm__("D_800A3598");
extern struct InputAction gMenuLeftActions[] __asm__("D_800A35A8");
extern struct InputAction gMenuRightActions[] __asm__("D_800A35B8");
extern struct InputAction gMenuAcceptActions[] __asm__("D_800A35C8");
extern struct InputAction gMenuBackActions[] __asm__("D_800A35D8");
extern struct InputAction gMenuStartActions[] __asm__("D_800A35E8");
extern u32 gPollPadButtons(u32) __asm__("func_8001EF8C");
extern void func_80077990(void);
extern void gSyncMenuFontBank(void) __asm__("func_800788B4");
extern void gSetupFrontEndMenus(s32) __asm__("func_80077AD4");
extern u8 gStartNextAttractDemo(void) __asm__("func_80078C88");
extern u8 gCheckMenuDirectionRepeat(s32 *, s32) __asm__("func_80082D30");
extern void gPlayMovieClip(u32, s32) __asm__("func_80082DDC");
extern void gDecodeButtonEdge(u32, struct InputAction *) __asm__("func_8005780C");
extern s8 gPlayerMenuDepth[] __asm__("D_8009F8C0");
extern u8 gTitleScreenConfirmEnabled __asm__("D_8009F46C");
extern u8 gMenuDepthHighlightShade __asm__("D_8009F3FB");
extern u8 gMenuHighlightColorCycle[] __asm__("D_8009B3D4");
extern u8 gMenuDepthHighlightRgb[] __asm__("D_8009B3E0");
extern u8 gHighlightShadeTable[] __asm__("D_8009F3F8");
extern u8 gUiHoldFlag __asm__("D_8009F8BD");
extern s32 gPendingRowCapture __asm__("D_8009F8C4");
extern s32 gDeferredTransition __asm__("D_8009F8C8");
extern volatile s32 gGameClock __asm__("D_8009FA60");
extern volatile u8 gFrameBarrier __asm__("D_8009FA3C");
struct GameContext { u8 pad00[12]; s32 type; u8 pad10[0x4c]; s32 stage; };
extern struct GameContext *gGameSession __asm__("D_8009FA94");
extern struct Menu gTitleScreenMenuDescriptor __asm__("D_800A38F8");
extern struct Menu gProfileNameEntryMenuDescriptor __asm__("D_800A36B8");
extern struct Menu gTwoPlayerLoadoutMenuDescriptor __asm__("D_800A3658");
extern struct Menu gOnePlayerLoadoutMenuDescriptor __asm__("D_800A3628");
extern struct Menu gTwoPlayerMissionSelectMenuDescriptor __asm__("D_800A37D8");
extern struct Menu gOnePlayerMissionSelectMenuDescriptor __asm__("D_800A37A8");
extern struct Menu gFrontEndTransitionMenuDescriptor __asm__("D_800A3688");
extern struct Menu *gFrontEndTransitionNextMenu[] __asm__("D_8009B8B8");
extern s32 gPlayNonpositionalSoundEffect(s32, s32) __asm__("func_80046C84");
extern s32 gPopMenuPanel(void) __asm__("func_80078740");
extern void gActivateMenuPanel(struct Menu *, s32) __asm__("func_80078650");
extern void func_8008227C(void) __asm__("func_8008227C");
extern void gSelectEnabledMenuItem(s8, s32, s8) __asm__("func_800787D8");
extern void gEmitTranslucentTileRect(u32, s32, s32, s32, s32, u8) __asm__("func_8001C214");
extern void gSetTextPaletteEntry(u32, u8, u8, u8) __asm__("func_800493C0");
extern void gEmitAtlasSpriteSimple(s32,s32,s32,s32,s32,s32,s32,s32,s32) __asm__("func_80082AB0");
extern void gDrawAlignedMultilineText(void *,s32,s32,s32,s32,s32,s32,s32) __asm__("func_800788F0");
extern void func_800826A0(void), func_80078B34(s32,s32,s32), func_8001B4A8(s32);
extern void gDrawSlotStatusLine(void) __asm__("func_80078B98");
void gUpdateAndDrawFrontEndMenu(void) __asm__("func_80078DD0");


/**
 *  @brief Updates controller-driven menu state and renders the active menu.
 *
 *  Polls two controllers, processes menu actions and callbacks, draws items,
 *  and handles inactivity and pending menu transitions when the menu is active.
 *  Each item is a 0x1C-byte record: text, action callback, next panel, icon,
 *  enabled/group bytes, coordinates and presentation fields. On confirm the
 *  selected item's handler receives action 3; its next-panel pointer then
 *  pushes a menu, or -1 pops one. A disabled item is skipped by func_800787D8.
 *  The draw pass uses an icon when present, otherwise text or a text ID, and
 *  calls each item's handler with action 0. The descriptor's +0x18 callback
 *  then runs once per active player. Labels backed by text IDs require the
 *  game resources.
 *  @par Globals
 *  D_8009F464 gates processing; D_8009F468 selects the menu; D_8009F8D0 and D_8009F8D4 hold button states.
 *  D_8009F890, D_8009F898, D_8009F8A0 and D_8009F8A8 track directional holds; D_800A3588 through D_800A35E8 hold action states.
 *  D_8009FA2C drives timing; D_8009F8C8 and D_8009F8C4 request transitions; D_8009FA94 supplies game context.
 *  @par Control flow
 *  Returns when disabled; otherwise loops over controllers and menu items, dispatches callbacks, and branches on menu mode and transition type.
 *  A pending transition can wait until D_8009FA3C clears.
 *  @par Calls
 *  func_8001EF8C polls input; func_8005780C and func_80082D30 update actions and repeats.
 *  func_80078650 and func_80078740 change menus; func_800787D8 moves selection; func_80046C84 plays menu sounds.
 *  func_8001C214, func_80082AB0 and func_800788F0 draw menu elements; menu and item callbacks provide custom behavior.
 *  @par Side effects
 *  Updates input, timing, selection and transition globals, invokes callbacks, and submits menu drawing and sound operations.
 *  @par Callers
 *  Confirmed: func_80027EE4.
 */
void gUpdateAndDrawFrontEndMenu(void)
{
    u32 buttons[2];
    s8 up[8], down[8], left[8], right[8], accept[8], back[8];
    s32 i, j, first, count, step, changed;
    s32 oldsel, shown, menu_x, x, y, width, color, n;
    struct Menu *menu;
    struct Item *item;
    u8 *rgb;
    if (gStillRunningState == 0) return;
    gPlayer1PrevMenuButtons = gPlayer1MenuButtons;
    buttons[0] = gPlayer1MenuButtons = gPollPadButtons(0);
    gPlayer2PrevMenuButtons = gPlayer2MenuButtons;
    buttons[1] = gPlayer2MenuButtons = gPollPadButtons(1);
    for (i = 0; i < 2; i++) {
        if (buttons[i] & 0x100000) { if (!gMenuUpHoldTicks[i]) gMenuUpHoldTicks[i] = gFrameTickCounter; } else gMenuUpHoldTicks[i] = 0;
        if (buttons[i] & 0x400000) { if (!gMenuDownHoldTicks[i]) gMenuDownHoldTicks[i] = gFrameTickCounter; } else gMenuDownHoldTicks[i] = 0;
        if (buttons[i] & 0x800000) { if (!gMenuLeftHoldTicks[i]) gMenuLeftHoldTicks[i] = gFrameTickCounter; } else gMenuLeftHoldTicks[i] = 0;
        if (buttons[i] & 0x200000) { if (!gMenuRightHoldTicks[i]) gMenuRightHoldTicks[i] = gFrameTickCounter; } else gMenuRightHoldTicks[i] = 0;
    }
    if (gStillRunningState != 1) {
        changed = count = step = first = 0;
        if (!gMenuSetupCalled) { func_80077990(); gMenuSetupCalled = 1; }
        if (gFontBankTable == &gMenuDescriptor) {
            for (j = 0; j < 2; j++) if (buttons[j] != prev_buttons[j]) goto reset_timer;
        } else {
reset_timer:
            gIdleDeadline = gFrameTickCounter + 9000;
        }
        if (gFrameTickCounter > gIdleDeadline) {
            u8 active = 0;
            if (gAttractDemoSequenceIndex) active = gStartNextAttractDemo();
            if (!active) {
                gFrontEndReturnMode = 5;
                gPlayMovieClip(gAttractMovieNames[gAttractMovieIndex], 0);
                gSetupFrontEndMenus(0);
                gAttractMovieIndex++;
                if (gAttractMovieIndex >= 2) { gAttractMovieIndex = 0; gAttractDemoSequenceIndex = 1; }
            }
        }
        i = 0;
        do {
            j = 0;
            do {
                *(s16 *)((u8 *)gSlotRecordTable + i * 8 + j * 4) = 0;
                j++;
            } while (j < 2);
            i++;
        } while (i < 2);
        gSyncMenuFontBank();
        if (gFontBankTable->filter) {
            gPlayer1MenuButtons = gFontBankTable->filter(gPlayer1MenuButtons, 0);
            gPlayer2MenuButtons = (*(u32 (**)(u32, s32))((u8 *)gFontBankTable + 12))(gPlayer2MenuButtons, 1);
        }
        for (j = 0; j < 2; j++) {
            u32 *btns;
            __asm__("lui %0,%%hi(D_8009F8D0)\n\taddiu %0,%0,%%lo(D_8009F8D0)" : "=r"(btns));
            gDecodeButtonEdge(btns[j], &gMenuUpActions[j]);
            gDecodeButtonEdge(btns[j], &gMenuDownActions[j]);
            gDecodeButtonEdge(btns[j], &gMenuLeftActions[j]);
            gDecodeButtonEdge(btns[j], &gMenuRightActions[j]);
            gDecodeButtonEdge(btns[j], &gMenuAcceptActions[j]);
            gDecodeButtonEdge(btns[j], &gMenuBackActions[j]);
            gDecodeButtonEdge(btns[j], &gMenuStartActions[j]);
            up[j] = 0;
            down[j] = 0;
            left[j] = 0;
            right[j] = 0;
            accept[j] = 0;
            back[j] = 0;
        }
        switch (gFontBankTable->mode) {
        case 0: first = 0; count = 2; step = first; break;
        case 1: first = 0; count = 1; step = first; break;
        case 2: first = 0; count = 2; step = 1; break;
        case 3: first = 1; count = 2; step = first; break;
        }
        {
            s32 idx;
            for (j = first, idx = first; j < count; j++, idx += step) {
                gActivePlayerIndex = j;
                if (!gMenuAcceptActions[j].phase) accept[idx] = 1;
                if (gFontBankTable->extra && !gMenuStartActions[j].phase) accept[idx] = 1;
                if (!gMenuBackActions[j].phase) back[idx] = 1;
                {
                    register u32 arr __asm__("$4");
                    if (!gMenuUpActions[j].phase || gCheckMenuDirectionRepeat((s32 *)((j << 2) + ({ __asm__("lui %0,%%hi(D_8009F890)\n\taddiu %0,%0,%%lo(D_8009F890)" : "=r"(arr)); arr; })), 2)) up[idx] = 1;
                    if (!gMenuDownActions[j].phase || gCheckMenuDirectionRepeat((s32 *)((j << 2) + ({ __asm__("lui %0,%%hi(D_8009F898)\n\taddiu %0,%0,%%lo(D_8009F898)" : "=r"(arr)); arr; })), 2)) down[idx] = 1;
                    if (!gMenuLeftActions[j].phase || gCheckMenuDirectionRepeat((s32 *)((j << 2) + ({ __asm__("lui %0,%%hi(D_8009F8A0)\n\taddiu %0,%0,%%lo(D_8009F8A0)" : "=r"(arr)); arr; })), 1)) left[idx] = 1;
                    if (!gMenuRightActions[j].phase || gCheckMenuDirectionRepeat((s32 *)((j << 2) + ({ __asm__("lui %0,%%hi(D_8009F8A8)\n\taddiu %0,%0,%%lo(D_8009F8A8)" : "=r"(arr)); arr; })), 1)) right[idx] = 1;
                }
            }
        }
        if (!step) count = 1;
        if (gUiInteractionEnabled) {
            for (i = 0; i < count && !changed; i++) {
                gActivePlayerIndex = i;
                if (accept[i]) {
                    struct Item *item = &gFontBankTable->items[gFontBankTable->selection[i]];
                    if (gFontBankTable != &gTitleScreenMenuDescriptor || gTitleScreenConfirmEnabled) gPlayNonpositionalSoundEffect(0x48, 0);
                    if (item->handler) {
                        menu = gFontBankTable;
                        item->handler(3, 0, 0, 0, menu->selection[i]);
                        if (gFontBankTable != menu) { changed = 1; continue; }
                    }
                    if (item->next) {
                        if (item->next == (struct Menu *)-1) {
                            if (gPopMenuPanel()) { changed = 1; continue; }
                        } else {
                            gActivateMenuPanel(item->next, 1);
                            changed = 1; continue;
                        }
                    }
                }
                if (back[i]) {
                    u32 cbase;
                    s8 *depth;
                    __asm__("lui %0,%%hi(D_8009F8C0)\n\taddiu %0,%0,%%lo(D_8009F8C0)" : "=r"(cbase) : "r"(i));
                    depth = (s8 *)i + cbase;
                    if (*depth) {
                        (*depth)--;
                        if (*depth < 0) *depth = 0;
                        else if (*depth == 1) gUiHoldFlag = 1;
                        gPlayNonpositionalSoundEffect(0x49, 0);
                        changed = 1;
                        if (gFontBankTable == &gProfileNameEntryMenuDescriptor) func_8008227C();
                        continue;
                    }
                    menu = gFontBankTable;
                    if (menu->cancel) {
                        menu->cancel();
                        if (gFontBankTable != menu) { changed = 1; continue; }
                    } else if (gPopMenuPanel()) {
                        gPlayNonpositionalSoundEffect(0x49, 0);
                        changed = 1; continue;
                    }
                }
                if (up[i]) {
                    u32 cbase;
                    __asm__("lui %0,%%hi(D_8009F8C0)\n\taddiu %0,%0,%%lo(D_8009F8C0)" : "=r"(cbase) : "r"(i));
                    if (!*((s8 *)i + cbase)) {
                        oldsel = gFontBankTable->selection[i];
                        gSelectEnabledMenuItem((s8)(gFontBankTable->selection[i] - 1), -1, (s8)i);
                        if (oldsel != gFontBankTable->selection[i]) gPlayNonpositionalSoundEffect(0x46, 0);
                    }
                    if (gFontBankTable->items[gFontBankTable->selection[i]].handler)
                        gFontBankTable->items[gFontBankTable->selection[i]].handler(4, 0, 0, 0, gFontBankTable->selection[i]);
                }
                if (down[i]) {
                    u32 cbase;
                    __asm__("lui %0,%%hi(D_8009F8C0)\n\taddiu %0,%0,%%lo(D_8009F8C0)" : "=r"(cbase) : "r"(i));
                    if (!*((s8 *)i + cbase)) {
                        oldsel = gFontBankTable->selection[i];
                        gSelectEnabledMenuItem((s8)(gFontBankTable->selection[i] + 1), 1, (s8)i);
                        if (oldsel != gFontBankTable->selection[i]) gPlayNonpositionalSoundEffect(0x46, 0);
                    }
                    if (gFontBankTable->items[gFontBankTable->selection[i]].handler)
                        gFontBankTable->items[gFontBankTable->selection[i]].handler(5, 0, 0, 0, gFontBankTable->selection[i]);
                }
                if (left[i]) {
                    if (gFontBankTable->items[gFontBankTable->selection[i]].handler)
                        gFontBankTable->items[gFontBankTable->selection[i]].handler(2, 0, 0, 0, gFontBankTable->selection[i]);
                }
                if (right[i]) {
                    if (gFontBankTable->items[gFontBankTable->selection[i]].handler)
                        gFontBankTable->items[gFontBankTable->selection[i]].handler(1, 0, 0, 0, gFontBankTable->selection[i]);
                }
            }
        }
        if (gUiHoldFlag) {
            for (i = 0; i < count; i++) {
                s32 menu_y;
                gActivePlayerIndex = i;
                n = 0;
                for (j = 0; j < gFontBankTable->item_count; j++) {
                    if (gFontBankTable->items[j].player == i && gFontBankTable->items[j].enabled) n++;
                }
                {
                    s32 h = (n << 2) + n;
                    width = h << 1;
                }
                menu_x = gFontBankTable->x;
                menu_y = gFontBankTable->y;
                if (menu_y + width >= 210) menu_y += 210 - (menu_y + width);
                if (i == 0 && gFontBankTable->width) {
                    gEmitTranslucentTileRect(0, (menu_x - 6) * 2, (menu_y - 5) * 2, gFontBankTable->width * 2, (width + 10) * 2, 1);
                }
                shown = menu_y;
                for (j = 0; j < gFontBankTable->item_count; j++) {
                    if (gFontBankTable->items[j].player != i) continue;
                    item = &gFontBankTable->items[j];
                    if (item->icon != -1) {
                        color = 0x80;
                        if (j == gFontBankTable->selection[i] && item->enabled) {
                            if (gPlayerMenuDepth[i]) color = gMenuDepthHighlightShade + 0x80;
                            else {
                                s32 frame = gGameClock;
                                __asm__("lui %0,%%hi(D_8009F3F8)\n\taddiu %0,%0,%%lo(D_8009F3F8)" : "=r"(rgb) : "r"(frame));
                                color = *(((u8 *)((frame >> 4) & 7)) + (u32)rgb) + 0x80;
                            }
                        }
                        x = item->x;
                        y = item->y;
                        gEmitAtlasSpriteSimple(item->icon, x, y, -1, -1, 10, color, item->flags & 7, item->flags & 0x38);
                    } else if (item->text || item->unk16) {
                        if (j == gFontBankTable->selection[i] && item->enabled) {
                            u8 *rgb2;
                            color = 0x11;
                            if (gPlayerMenuDepth[i]) rgb2 = gMenuDepthHighlightRgb;
                            else rgb2 = &gMenuHighlightColorCycle[((gGameClock >> 4) & 7) * 3];
                            gSetTextPaletteEntry(color, rgb2[0], rgb2[1], rgb2[2]);
                        } else {
                            color = 15;
                            if (item->color != -1) color = item->color;
                        }
                        x = item->x ? item->x : menu_x;
                        y = item->y ? item->y : shown;
                        gDrawAlignedMultilineText(item->text, x, y, color, item->unk16, item->flags & 7, item->flags & 0x38, item->unk15);
                        shown += 10;
                    } else {
                        color = 2;
                        x = menu_x;
                        y = shown;
                    }
                    if (item->handler) item->handler(0, x, y, color, j);
                }
            }
        }
        for (i = 0; i < count; i++) {
            gActivePlayerIndex = i;
            if (gFontBankTable->update) gFontBankTable->update(i);
        }
        func_800826A0();
        if (gDeferredTransition == -1) gDrawSlotStatusLine();
        else { func_80078B34(0, 0, 0); func_80078B34(0, 1, 0); }
        if (gDeferredTransition != -1) {
            gUiHoldFlag = 1;
            if (gPendingRowCapture) {
                func_80078B34(500, 0, 0);
                gDrawSlotStatusLine();
                func_8001B4A8(1);
                while (gFrameBarrier) { }
                gPlayMovieClip(gPendingRowCapture, 1);
            }
            switch (gDeferredTransition) {
            case 1:
                if (gGameSession->type == 2) {
                    if (gGameSession->stage == 1) {
                        if (gPendingRowCapture) {
                            gFrontEndTransitionNextMenu[0] = &gTwoPlayerLoadoutMenuDescriptor;
                            gActivateMenuPanel(&gFrontEndTransitionMenuDescriptor, 1);
                        } else gActivateMenuPanel(&gTwoPlayerLoadoutMenuDescriptor, 1);
                    } else gActivateMenuPanel(&gTwoPlayerLoadoutMenuDescriptor, 1);
                } else {
                    if (gGameSession->stage == 1) {
                        if (gPendingRowCapture) {
                            gFrontEndTransitionNextMenu[0] = &gOnePlayerLoadoutMenuDescriptor;
                            gActivateMenuPanel(&gFrontEndTransitionMenuDescriptor, 1);
                        } else gActivateMenuPanel(&gOnePlayerLoadoutMenuDescriptor, 1);
                    } else gActivateMenuPanel(&gOnePlayerLoadoutMenuDescriptor, 1);
                }
                break;
            case 4:
                if (gGameSession->type == 2) gActivateMenuPanel(&gTwoPlayerMissionSelectMenuDescriptor, 1);
                else gActivateMenuPanel(&gOnePlayerMissionSelectMenuDescriptor, 1);
                break;
            case 5:
                if (gFontBankTable == &gMenuDescriptor) {
                    if (gPendingRowCapture) {
                        if (gGameSession->type == 2) gFrontEndTransitionNextMenu[0] = &gTwoPlayerMissionSelectMenuDescriptor;
                        else gFrontEndTransitionNextMenu[0] = &gOnePlayerMissionSelectMenuDescriptor;
                        gActivateMenuPanel(&gFrontEndTransitionMenuDescriptor, 1);
                    } else {
                        if (gGameSession->type == 2) gActivateMenuPanel(&gTwoPlayerMissionSelectMenuDescriptor, 1);
                        else gActivateMenuPanel(&gOnePlayerMissionSelectMenuDescriptor, 1);
                    }
                } else {
                    gFrontEndTransitionNextMenu[0] = (struct Menu *)-1;
                    gActivateMenuPanel(&gFrontEndTransitionMenuDescriptor, 1);
                }
                break;
            }
            gDeferredTransition = -1;
            gPendingRowCapture = 0;
        }
    }
    gPlayer1MenuButtons = buttons[0];
    gPlayer2MenuButtons = buttons[1];
}
