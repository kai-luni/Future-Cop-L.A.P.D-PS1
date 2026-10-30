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

extern struct MenuText260B8 *D_8009F2CC;
extern s8 D_8009F2D4;
extern s8 D_8009F2D5;
extern s32 D_8009FA2C;
extern s32 D_8009FA6C;
extern s32 D_8009FA00;
extern s32 D_8009FAC0;
extern s8 D_8009ABE8[];
extern s8 D_8009ABEC[];
extern s8 D_8009ABED[];
extern s32 D_8009FA58[];
extern s32 D_8009F670[];
extern s32 D_8009F678[2];
extern s32 D_8009FA78[2];
extern struct InputAction260B8 D_800A0AD0[];
extern struct InputAction260B8 D_800A0AE0[];
extern struct InputAction260B8 D_800A0AB0[];
extern struct InputAction260B8 D_800A0AC0[];
extern struct InputAction260B8 D_800A0B20[];

extern void func_8005780C(u32 buttons, struct InputAction260B8 *action);
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

    menu = D_8009F2CC;
    base1 = menu->unk1C + D_8009F2D4;
    base2 = menu->unk20 + D_8009F2D5;
    if (D_8009FA2C & 0x40) {
        initialColor = 0x13;
    }
    *base2 = initialColor;
    *base1 = initialColor;

    state = obj->state;

    actionOffset = (u32)slotArg * 8;
    p1 = (struct InputAction260B8 *)(actionOffset + (u32)D_800A0AD0);
    func_8005780C(state->buttons, p1);
    p2 = (struct InputAction260B8 *)(actionOffset + (u32)D_800A0AE0);
    func_8005780C(state->buttons, p2);
    p3 = (struct InputAction260B8 *)((u32)D_800A0AB0 + actionOffset);
    func_8005780C(state->buttons, p3);
    p4 = (struct InputAction260B8 *)(actionOffset + (u32)D_800A0AC0);
    func_8005780C(state->buttons, p4);

    x = (selection - 0xDC) / 2;
    items = D_8009ABE8;
    func_8001C214(0x5294, x - 2, 0x9E, 0xE0, 0xB6, 1);
    func_8001C214(0, x, 0xA0, 0xDC, 0xB2, 1);

    selection = state->selected;

    if (p3->phase == 0) {
        items[5] = 6;
        func_80046C84(0x3F, D_8009FA58[slotArg]);
        if (--selection < 0) {
            selection = 5;
        }
    } else if (p4->phase == 0) {
        items[5] = 6;
        func_80046C84(0x3F, D_8009FA58[slotArg]);
        if (++selection >= 6) {
            selection = 0;
        }
    } else if (active) {
        switch (code = items[selection]) {
        case 0:
            func_80046C84(0x3E, D_8009FA58[slotArg]);
            selection = -1;
            state->selected = selection;
            D_8009FA6C--;
            state->selected = selection;
            goto display;
        case 7:
            func_80046C84(0x3E, D_8009FA58[slotArg]);
            items[5] = 6;
            goto store_selected;
        case 8:
            saveOffset = (u32)slotArg * 4;
            func_80046C84(0x3E, *(s32 *)(saveOffset + (u32)D_8009FA58));
            if ((func_80060A0C(*(s32 *)(saveOffset + (u32)D_8009F670), slotArg) & 0xFF) != 0) {
                D_8009FA00 = 5;
            } else {
                flag = 1;
            }
            D_8009ABED[0] = 6;
            D_8009F670[slotArg] = 0;
            goto store_selected;
        case 6:
            func_80046C84(0x3E, D_8009FA58[slotArg]);
            items[5] = 7;
            goto store_selected;
        default:
            state->selected = selection;
            goto display;
        }
    } else {
        backAction = (struct InputAction260B8 *)(actionOffset + (u32)D_800A0B20);
        if (backAction->phase == 0) {
            func_80046C84(0x3E, D_8009FA58[slotArg]);
            selection = -1;
            items[5] = 6;
            state->selected = selection;
            D_8009FA6C--;
            if (D_8009F678[slotArg] != 0) {
                func_80046A34(D_8009F678[slotArg]);
                D_8009F678[slotArg] = 0;
            }
            goto store_selected;
        }
        if (p2->phase == 3 || (leftPhase = p1->phase) == 3) {
            delta = (p2->phase == 3) ? 1 : -1;
            switch (code = items[selection]) {
            case 1:
                func_800451F8(func_8004522C() + delta);
                func_80025E18(D_8009FAC0, func_8004522C(), slotArg);
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
                func_80046C84(0x3F, D_8009FA58[slotArg]);
                D_8009ABEC[0] = 5;
                goto store_selected;
            case 5:
                func_800451B4(1);
                func_80046C84(0x3F, D_8009FA58[slotArg]);
                items[4] = 4;
                goto store_selected;
            case 7:
                func_80046C84(0x3F, D_8009FA58[slotArg]);
                D_8009ABED[0] = 8;
                goto store_selected;
            case 8:
                toggleOffset = (u32)slotArg * 4;
                func_80046C84(0x3F, *(s32 *)(toggleOffset + (u32)D_8009FA58));
                D_8009ABED[0] = 7;
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
    if (selection != -1 && D_8009F678[slotArg] != 0) {
        switch (code = items[selection]) {
        case 1:
            id = D_8009FAC0;
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
        if (D_8009FA78[slotArg] != id) {
            func_80046A34(D_8009F678[slotArg]);
            D_8009F678[slotArg] = 0;
        }
    }

    x = x / 2 + 6;
    textY = 0x53;
    for (i = 0; (code = items[i]) >= 0; i++) {
        textOffset = (u32)code * 4;
        if (flag != 0) {
            text = D_8009F2CC->unk24;
        } else {
            text = *(u8 **)(textOffset + (u32)D_8009F2CC);
        }
        func_80049100(text, x, textY,
                      i == selection ? 8 : (flag != 0 ? 0xB : 0x10));
        textY += 0xE;
    }
}
