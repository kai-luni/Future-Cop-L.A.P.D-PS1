// Categories: Game Engine > Options Menu | Graphics > 2D Drawing > Text | Input > Menu Navigation
#include "game/types.h"

struct InputAction260B8 {
    u32 buttons;
    s32 phase;
};

struct MenuText260B8 {
    u8 *text[7];
    u8 *unk1C;
    u8 *unk20;
    u8 *unk24;
};

struct State260B8 {
    u8 pad0[0x30];
    u32 buttons;
    u8 pad34[0x312 - 0x34];
    s8 selected;
};

struct Obj260B8 {
    u8 pad0[0xAC];
    struct State260B8 *state;
};

extern struct MenuText260B8 *gLabelRecord __asm__("D_8009F2CC");
extern s8 D_8009F2D4;
extern s8 D_8009F2D5;
extern s32 gFrameTickCounter __asm__("D_8009FA2C");
extern s32 gPanelSelection __asm__("D_8009FA6C");
extern s32 gScreenState __asm__("D_8009FA00");
extern s32 gPanelSoundIds __asm__("D_8009FAC0");
extern s8 gItemCodeTable[] __asm__("D_8009ABE8");
extern s8 D_8009ABEC[];
extern s8 gItemLabelByte[] __asm__("D_8009ABED");
extern s32 gPlayerEntities[] __asm__("D_8009FA58");
extern s32 D_8009F670[];
extern s32 gSlotVoiceHandle[2] __asm__("D_8009F678");
extern s32 gSlotTrackId[2] __asm__("D_8009FA78");
extern struct InputAction260B8 D_800A0AD0[];
extern struct InputAction260B8 D_800A0AE0[];
extern struct InputAction260B8 D_800A0AB0[];
extern struct InputAction260B8 D_800A0AC0[];
extern struct InputAction260B8 gBackActionRecord[] __asm__("D_800A0B20");

extern void gDecodeButtonEdge(u32 buttons, struct InputAction260B8 *action) __asm__("func_8005780C");
extern void func_8001C214(u32 color, s32 x, s32 y, s32 w, s32 h, u8 blend);
extern s32 func_80046C84(s32 id, s32 arg1);
extern u32 func_80060A0C(s32 arg0, s32 arg1);
extern void func_80046A34(s32 handle);
extern void func_800451B4(s32 enable);
extern void func_800451F8(s32 volume);
extern s32 func_8004522C(void);
extern void func_80045238(s32 volume);
extern s32 func_80045288(void);
extern void func_80045294(s32 volume);
extern s32 func_800452C8(void);
extern void func_80025E18(s32 id, s32 value, s32 slot);
extern void func_80025F58(void);
extern void func_80049100(u8 *text, s32 x, s32 y, s32 color);

/**
 * @brief Draws and services one player's options sub-panel: the item list, the
 *        navigation, the volume and toggle rows, and the confirm row.
 *
 * @param obj      Object whose state field (+0xAC) holds the shared button word
 *                 (+0x30) and the selected row (+0x312).
 * @param slotArg  Player slot, 0-based; indexes the four input action tables, the
 *                 per-slot object ids in D_8009FA58 and the sound handles in
 *                 D_8009F678.
 * @param arg2     Not referenced by the body; kept for signature parity with the
 *                 caller.
 * @param selectionArg Panel base offset, 0x140 or 0x280 from the caller; only
 *                 (selectionArg - 0xDC) / 2 survives, the horizontal origin.
 * @param active   Non-zero when this tab owns the confirm handling: the press edge
 *                 then acts on the selected row, otherwise the same edge toggles
 *                 that row and the left/right edges adjust the volumes.
 *
 * @return Nothing.
 *
 * @par Confirmed control flow
 * 1. Two blended rectangles are drawn through func_8001C214: a backdrop
 *    (colour 0x5294) at x - 2, 0x9E, 0xE0 x 0xB6 and an inner panel (colour 0) at
 *    x, 0xA0, 0xDC x 0xB2, both 2x scale and blend mode 1. The byte at
 *    menu->unk1C + D_8009F2D4 and the byte at menu->unk20 + D_8009F2D5 are both set
 *    to 0x0F, or to 0x13 when D_8009FA2C has bit 0x40.
 * 2. Four action records (D_800A0AB0, D_800A0AC0, D_800A0AD0, D_800A0AE0, each
 *    indexed by slot * 8) are refreshed from the button word with func_8005780C.
 *    Phase 0 is the press edge, 3 the held state, so navigation uses 0 and the
 *    volume rows accept 0 as well as 3 for auto-repeat.
 * 3. The selection is the signed byte at +0x312, clamped to 0..5: press up
 *    (D_800A0AB0) decrements with wrap to 5, press down (D_800A0AC0) increments
 *    with wrap to 0; both play sound 0x3F.
 * 4. The six item codes come from D_8009ABE8. On the press edge with @c active:
 *    code 0 leaves the panel (sound 0x3E, selection -1, D_8009FA6C--), 7 and 6 swap
 *    the label byte at D_8009ABED between 8 and 7, 8 calls func_80060A0C on
 *    D_8009F670[slot] and sets screen state D_8009FA00 = 5 on success or switches
 *    the whole list to the greyed-out message otherwise, and any other code simply
 *    stores the row.
 * 5. With @c active clear: the back action (D_800A0B20, phase 0) closes the panel
 *    and releases the preview sound handle; the left/right records with phase 3 or
 *    edge adjust the three volumes through func_800451F8/func_80045238/
 *    func_80045294 and restart the preview via func_80025E18 with track
 *    D_8009FAC0, 0x2F or 0x30, then func_80025F58 re-applies the mix; on the press
 *    edge code 4 disables sound, 5 enables it (func_800451B4), and 7/8 swap the
 *    label at D_8009ABED.
 * 6. The list is printed with func_80049100 from x/2 + 6, starting at y 0x53 in
 *    steps of 0x0E; the selected row uses colour 8, the greyed list colour 0x0B and
 *    all other rows 0x10. When the greyed message is active every row is drawn with
 *    menu->unk24 instead of the per-code text pointer.
 *
 * @par Likely role
 * The per-player options page of the pre-race or pause front end: three sound
 * volumes with audible preview, a sound on/off row, a two-state row without
 * immediate side effects, and a confirm row that either advances the game state or
 * shows a "not available" message.
 *
 * @par Unknown
 * - What the two-state row (codes 6/7) actually configures: it only swaps its own
 *   label byte and never touches a global.
 * - Whether D_8009F670 holds a configuration or vehicle index; the confirm row
 *   only hands it to func_80060A0C.
 *
 * @par Callers
 * func_800267C8 (the parent page, which calls this function only when the shared
 * selection is 0 or greater, i.e. while the sub-panel is open).
 */
void func_800260B8(struct Obj260B8 *obj, s32 slotArg, s32 arg2, s32 selectionArg, s32 active)
{
    s32 flag;
    register struct State260B8 *state __asm__("$22");
    register u32 actionOffset __asm__("$17");
    struct InputAction260B8 *p1;
    struct InputAction260B8 *p2;
    register struct InputAction260B8 *p3 __asm__("$16");
    struct InputAction260B8 *p4;
    u8 *base1;
    u8 *base2;
    u8 *text;
    s8 *items;
    s32 selection;
    s32 x;
    s32 i;
    register s32 code __asm__("$4");
    register s32 initialColor __asm__("$6");
    register struct MenuText260B8 *menu __asm__("$5");
    s32 color;
    s32 delta;
    s32 id;
    s32 textY;
    register u32 textOffset __asm__("$2");
    register s32 leftPhase __asm__("$2");
    register struct InputAction260B8 *backAction __asm__("$2");
    register u32 saveOffset __asm__("$16");
    register u32 toggleOffset __asm__("$3");

    flag = 0;
    selection = selectionArg;
    initialColor = 0xF;

    menu = gLabelRecord;
    base1 = menu->unk1C + D_8009F2D4;
    base2 = menu->unk20 + D_8009F2D5;
    if (gFrameTickCounter & 0x40) {
        initialColor = 0x13;
    }
    *base2 = initialColor;
    *base1 = initialColor;

    state = obj->state;

    actionOffset = (u32)slotArg * 8;
    p1 = (struct InputAction260B8 *)(actionOffset + (u32)D_800A0AD0);
    gDecodeButtonEdge(state->buttons, p1);
    p2 = (struct InputAction260B8 *)(actionOffset + (u32)D_800A0AE0);
    gDecodeButtonEdge(state->buttons, p2);
    p3 = (struct InputAction260B8 *)((u32)D_800A0AB0 + actionOffset);
    gDecodeButtonEdge(state->buttons, p3);
    p4 = (struct InputAction260B8 *)(actionOffset + (u32)D_800A0AC0);
    gDecodeButtonEdge(state->buttons, p4);

    x = (selection - 0xDC) / 2;
    items = gItemCodeTable;
    func_8001C214(0x5294, x - 2, 0x9E, 0xE0, 0xB6, 1);
    func_8001C214(0, x, 0xA0, 0xDC, 0xB2, 1);

    selection = state->selected;

    if (p3->phase == 0) {
        items[5] = 6;
        func_80046C84(0x3F, gPlayerEntities[slotArg]);
        if (--selection < 0) {
            selection = 5;
        }
    } else if (p4->phase == 0) {
        items[5] = 6;
        func_80046C84(0x3F, gPlayerEntities[slotArg]);
        if (++selection >= 6) {
            selection = 0;
        }
    } else if (active) {
        switch (code = items[selection]) {
        case 0:
            func_80046C84(0x3E, gPlayerEntities[slotArg]);
            selection = -1;
            state->selected = selection;
            gPanelSelection--;
            state->selected = selection;
            goto display;
        case 7:
            func_80046C84(0x3E, gPlayerEntities[slotArg]);
            items[5] = 6;
            goto store_selected;
        case 8:
            saveOffset = (u32)slotArg * 4;
            func_80046C84(0x3E, *(s32 *)(saveOffset + (u32)gPlayerEntities));
            if ((func_80060A0C(*(s32 *)(saveOffset + (u32)D_8009F670), slotArg) & 0xFF) != 0) {
                gScreenState = 5;
            } else {
                flag = 1;
            }
            gItemLabelByte[0] = 6;
            D_8009F670[slotArg] = 0;
            goto store_selected;
        case 6:
            func_80046C84(0x3E, gPlayerEntities[slotArg]);
            items[5] = 7;
            goto store_selected;
        default:
            state->selected = selection;
            goto display;
        }
    } else {
        backAction = (struct InputAction260B8 *)(actionOffset + (u32)gBackActionRecord);
        if (backAction->phase == 0) {
            func_80046C84(0x3E, gPlayerEntities[slotArg]);
            selection = -1;
            items[5] = 6;
            state->selected = selection;
            gPanelSelection--;
            if (gSlotVoiceHandle[slotArg] != 0) {
                func_80046A34(gSlotVoiceHandle[slotArg]);
                gSlotVoiceHandle[slotArg] = 0;
            }
            goto store_selected;
        }
        if (p2->phase == 3 || (leftPhase = p1->phase) == 3) {
            delta = (p2->phase == 3) ? 1 : -1;
            switch (code = items[selection]) {
            case 1:
                func_800451F8(func_8004522C() + delta);
                func_80025E18(gPanelSoundIds, func_8004522C(), slotArg);
                break;
            case 2:
                func_80045238(func_80045288() + delta);
                func_80025E18(0x2F, func_80045288(), slotArg);
                break;
            case 3:
                func_80045294(func_800452C8() + delta);
                func_80025E18(0x30, func_800452C8(), slotArg);
                break;
            default:
                break;
            }
            func_80025F58();
            state->selected = selection;
            goto display;
        } else if (p2->phase == 0 || leftPhase == 0) {
            switch (code = items[selection]) {
            case 4:
                func_800451B4(0);
                func_80046C84(0x3F, gPlayerEntities[slotArg]);
                D_8009ABEC[0] = 5;
                goto store_selected;
            case 5:
                func_800451B4(1);
                func_80046C84(0x3F, gPlayerEntities[slotArg]);
                items[4] = 4;
                goto store_selected;
            case 7:
                func_80046C84(0x3F, gPlayerEntities[slotArg]);
                gItemLabelByte[0] = 8;
                goto store_selected;
            case 8:
                toggleOffset = (u32)slotArg * 4;
                func_80046C84(0x3F, *(s32 *)(toggleOffset + (u32)gPlayerEntities));
                gItemLabelByte[0] = 7;
                goto store_selected;
            default:
                state->selected = selection;
                goto display;
            }
        } else {
            goto store_selected;
        }
    }

store_selected:
    state->selected = selection;

display:
    if (selection != -1 && gSlotVoiceHandle[slotArg] != 0) {
        switch (code = items[selection]) {
        case 1:
            id = gPanelSoundIds;
            break;
        case 2:
            id = 0x2F;
            break;
        case 3:
            id = 0x30;
            break;
        default:
            id = 0;
            break;
        }
        if (gSlotTrackId[slotArg] != id) {
            func_80046A34(gSlotVoiceHandle[slotArg]);
            gSlotVoiceHandle[slotArg] = 0;
        }
    }

    x = x / 2 + 6;
    textY = 0x53;
    for (i = 0; (code = items[i]) >= 0; i++) {
        textOffset = (u32)code * 4;
        if (flag != 0) {
            text = gLabelRecord->unk24;
        } else {
            text = *(u8 **)(textOffset + (u32)gLabelRecord);
        }
        func_80049100(text, x, textY,
                      i == selection ? 8 : (flag != 0 ? 0xB : 0x10));
        textY += 0xE;
    }
}
