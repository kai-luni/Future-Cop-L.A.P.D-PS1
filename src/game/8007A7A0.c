#include "game/types.h"

struct MenuState {
    u8 pad00[0xC];
    s32 mode;
    u8 pad10[0x4C];
    s32 active;
};

extern struct MenuState *D_8009FA94[];
extern s32 D_8009F8B8;
extern u8 D_8009F3C1;
extern s32 D_8009F8F0[];
extern u16 D_8009BEEC[];
extern s32 func_80046C84(s32 id, s32 arg1);
extern void func_800788F0(u8 *text, s32 x, s32 y, s32 color, s32 textId,
                         s32 alignX, s32 alignY, s32 font);

void func_8007A7A0(s32 command, s32 x, s32 y)
{
    s32 index;
    s32 textId;

    switch (command) {
    case 1:
    {
        register s32 *entry __asm__("$3");
        index = 0;
        if (D_8009FA94[0]->active != 0) {
            index = D_8009F8B8;
        }
        func_80046C84(0x47, 0);
        entry = D_8009F8F0;
        entry = (s32 *)((index << 2) + (s32)entry);
        (*entry)--;
        if (*entry < 0) {
            if (D_8009F3C1 != 0) {
                *entry = 3;
            } else {
                *entry = 2;
            }
        }
        break;
    }
    case 2:
    {
        register s32 *base __asm__("$3");
        register s32 *entry __asm__("$4");
        register s32 value __asm__("$2");
        register s32 offset __asm__("$2");
        s32 flag;
        s32 below;
        index = 0;
        if (D_8009FA94[0]->active != 0) {
            index = D_8009F8B8;
        }
        func_80046C84(0x47, 0);
        base = D_8009F8F0;
        offset = index << 2;
        entry = (s32 *)(offset + (s32)base);
        value = *entry;
        flag = D_8009F3C1;
        value++;
        *entry = value;
        if (flag != 0) {
            below = value < 4;
        } else {
            below = value < 3;
        }
        if (!below) {
            *entry = 0;
        }
        break;
    }
    case 0:
    {
        register s32 *base __asm__("$3");
        register u16 *labels __asm__("$3");
        register u8 *text __asm__("$4");
        register s32 selection __asm__("$2");
        selection = D_8009F8B8;
        __asm__("" : "+r"(selection));
        base = D_8009F8F0;
        __asm__("" : "+r"(base));
        switch (base[selection]) {
        case 2:
            textId = 0;
            break;
        case 1:
            textId = 1;
            break;
        case 0:
            textId = 2;
            break;
        case 3:
            textId = 3;
            break;
        default:
            textId = -1;
            break;
        }
        if (D_8009FA94[0]->active != 1 || D_8009FA94[0]->mode != 2) {
            if (D_8009F8B8 != 0) {
                textId = -1;
            }
            x = 0xA0;
        }
        if (textId != -1) {
            text = 0;
            __asm__("" : "+r"(text));
            labels = D_8009BEEC;
            __asm__("" : "+r"(labels));
            func_800788F0(text, x, y + 0xC, 0xF, labels[textId], 4, 8, 1);
        }
        break;
    }
    }
}
