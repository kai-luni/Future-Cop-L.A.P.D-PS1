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
extern s32 D_8009F464;
extern struct Menu *D_8009F468;
extern u8 D_8009F8CC, D_8009F3F2, D_8009F3F3, D_8009F45A, D_8009F8BE;
extern s32 D_8009F870, D_8009F8B0, D_8009F8B4, D_8009F8B8;
extern s32 prev_buttons[2] __asm__("D_8009F8B0");
extern u32 D_8009F8D0, D_8009F8D4;
extern volatile u32 D_8009F8D0_arr[] __asm__("D_8009F8D0");
extern s32 D_8009F890[], D_8009F898[], D_8009F8A0[], D_8009F8A8[];
extern volatile s32 D_8009FA2C;
extern u32 D_8009B324[];
extern struct Menu D_800A3808;
extern s16 D_800A3578[2][4];
extern struct InputAction D_800A3588[], D_800A3598[], D_800A35A8[], D_800A35B8[], D_800A35C8[], D_800A35D8[], D_800A35E8[];
extern u32 func_8001EF8C(u32);
extern void func_80077990(void), func_800788B4(void), func_80077AD4(s32);
extern u8 func_80078C88(void), func_80082D30(s32 *, s32);
extern void func_80082DDC(u32, s32), func_8005780C(u32, struct InputAction *);
extern s8 D_8009F8C0[];
extern u8 D_8009F8BD, D_8009F46C, D_8009F3FB, D_8009F3F8[], D_8009B3D4[], D_8009B3E0[];
extern s32 D_8009F8C4, D_8009F8C8;
extern volatile s32 D_8009FA60;
extern volatile u8 D_8009FA3C;
struct GameContext { u8 pad00[12]; s32 type; u8 pad10[0x4c]; s32 stage; };
extern struct GameContext *D_8009FA94;
extern struct Menu D_800A38F8, D_800A36B8, D_800A3658, D_800A3628, D_800A37D8, D_800A37A8, D_800A3688;
extern struct Menu *D_8009B8B8[];
extern s32 func_80046C84(s32, s32), func_80078740(void);
extern void func_80078650(struct Menu *, s32), func_8008227C(void), func_800787D8(s8, s32, s8);
extern void func_8001C214(u32, s32, s32, s32, s32, u8);
extern void func_800493C0(u32, u8, u8, u8);
extern void func_80082AB0(s32,s32,s32,s32,s32,s32,s32,s32,s32);
extern void func_800788F0(void *,s32,s32,s32,s32,s32,s32,s32);
extern void func_800826A0(void), func_80078B98(void), func_80078B34(s32,s32,s32), func_8001B4A8(s32);

void func_80078DD0(void)
{
    u32 buttons[2];
    s8 up[8], down[8], left[8], right[8], accept[8], back[8];
    s32 i, j, first, count, step, changed;
    s32 oldsel, shown, menu_x, x, y, width, color, n;
    struct Menu *menu;
    struct Item *item;
    u8 *rgb;
    if (D_8009F464 == 0) return;
    D_8009F8B0 = D_8009F8D0;
    buttons[0] = D_8009F8D0 = func_8001EF8C(0);
    D_8009F8B4 = D_8009F8D4;
    buttons[1] = D_8009F8D4 = func_8001EF8C(1);
    for (i = 0; i < 2; i++) {
        if (buttons[i] & 0x100000) { if (!D_8009F890[i]) D_8009F890[i] = D_8009FA2C; } else D_8009F890[i] = 0;
        if (buttons[i] & 0x400000) { if (!D_8009F898[i]) D_8009F898[i] = D_8009FA2C; } else D_8009F898[i] = 0;
        if (buttons[i] & 0x800000) { if (!D_8009F8A0[i]) D_8009F8A0[i] = D_8009FA2C; } else D_8009F8A0[i] = 0;
        if (buttons[i] & 0x200000) { if (!D_8009F8A8[i]) D_8009F8A8[i] = D_8009FA2C; } else D_8009F8A8[i] = 0;
    }
    if (D_8009F464 != 1) {
        changed = count = step = first = 0;
        if (!D_8009F8CC) { func_80077990(); D_8009F8CC = 1; }
        if (D_8009F468 == &D_800A3808) {
            for (j = 0; j < 2; j++) if (buttons[j] != prev_buttons[j]) goto reset_timer;
        } else {
reset_timer:
            D_8009F870 = D_8009FA2C + 9000;
        }
        if (D_8009FA2C > D_8009F870) {
            u8 active = 0;
            if (D_8009F3F2) active = func_80078C88();
            if (!active) {
                D_8009F45A = 5;
                func_80082DDC(D_8009B324[D_8009F3F3], 0);
                func_80077AD4(0);
                D_8009F3F3++;
                if (D_8009F3F3 >= 2) { D_8009F3F3 = 0; D_8009F3F2 = 1; }
            }
        }
        i = 0;
        do {
            j = 0;
            do {
                *(s16 *)((u8 *)D_800A3578 + i * 8 + j * 4) = 0;
                j++;
            } while (j < 2);
            i++;
        } while (i < 2);
        func_800788B4();
        if (D_8009F468->filter) {
            D_8009F8D0 = D_8009F468->filter(D_8009F8D0, 0);
            D_8009F8D4 = (*(u32 (**)(u32, s32))((u8 *)D_8009F468 + 12))(D_8009F8D4, 1);
        }
        for (j = 0; j < 2; j++) {
            u32 *btns;
            __asm__("lui %0,%%hi(D_8009F8D0)\n\taddiu %0,%0,%%lo(D_8009F8D0)" : "=r"(btns));
            func_8005780C(btns[j], &D_800A3588[j]);
            func_8005780C(btns[j], &D_800A3598[j]);
            func_8005780C(btns[j], &D_800A35A8[j]);
            func_8005780C(btns[j], &D_800A35B8[j]);
            func_8005780C(btns[j], &D_800A35C8[j]);
            func_8005780C(btns[j], &D_800A35D8[j]);
            func_8005780C(btns[j], &D_800A35E8[j]);
            up[j] = 0;
            down[j] = 0;
            left[j] = 0;
            right[j] = 0;
            accept[j] = 0;
            back[j] = 0;
        }
        switch (D_8009F468->mode) {
        case 0: first = 0; count = 2; step = first; break;
        case 1: first = 0; count = 1; step = first; break;
        case 2: first = 0; count = 2; step = 1; break;
        case 3: first = 1; count = 2; step = first; break;
        }
        {
            s32 idx;
            for (j = first, idx = first; j < count; j++, idx += step) {
                D_8009F8B8 = j;
                if (!D_800A35C8[j].phase) accept[idx] = 1;
                if (D_8009F468->extra && !D_800A35E8[j].phase) accept[idx] = 1;
                if (!D_800A35D8[j].phase) back[idx] = 1;
                {
                    register u32 arr __asm__("$4");
                    if (!D_800A3588[j].phase || func_80082D30((s32 *)((j << 2) + ({ __asm__("lui %0,%%hi(D_8009F890)\n\taddiu %0,%0,%%lo(D_8009F890)" : "=r"(arr)); arr; })), 2)) up[idx] = 1;
                    if (!D_800A3598[j].phase || func_80082D30((s32 *)((j << 2) + ({ __asm__("lui %0,%%hi(D_8009F898)\n\taddiu %0,%0,%%lo(D_8009F898)" : "=r"(arr)); arr; })), 2)) down[idx] = 1;
                    if (!D_800A35A8[j].phase || func_80082D30((s32 *)((j << 2) + ({ __asm__("lui %0,%%hi(D_8009F8A0)\n\taddiu %0,%0,%%lo(D_8009F8A0)" : "=r"(arr)); arr; })), 1)) left[idx] = 1;
                    if (!D_800A35B8[j].phase || func_80082D30((s32 *)((j << 2) + ({ __asm__("lui %0,%%hi(D_8009F8A8)\n\taddiu %0,%0,%%lo(D_8009F8A8)" : "=r"(arr)); arr; })), 1)) right[idx] = 1;
                }
            }
        }
        if (!step) count = 1;
        if (D_8009F8BE) {
            for (i = 0; i < count && !changed; i++) {
                D_8009F8B8 = i;
                if (accept[i]) {
                    struct Item *item = &D_8009F468->items[D_8009F468->selection[i]];
                    if (D_8009F468 != &D_800A38F8 || D_8009F46C) func_80046C84(0x48, 0);
                    if (item->handler) {
                        menu = D_8009F468;
                        item->handler(3, 0, 0, 0, menu->selection[i]);
                        if (D_8009F468 != menu) { changed = 1; continue; }
                    }
                    if (item->next) {
                        if (item->next == (struct Menu *)-1) {
                            if (func_80078740()) { changed = 1; continue; }
                        } else {
                            func_80078650(item->next, 1);
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
                        else if (*depth == 1) D_8009F8BD = 1;
                        func_80046C84(0x49, 0);
                        changed = 1;
                        if (D_8009F468 == &D_800A36B8) func_8008227C();
                        continue;
                    }
                    menu = D_8009F468;
                    if (menu->cancel) {
                        menu->cancel();
                        if (D_8009F468 != menu) { changed = 1; continue; }
                    } else if (func_80078740()) {
                        func_80046C84(0x49, 0);
                        changed = 1; continue;
                    }
                }
                if (up[i]) {
                    u32 cbase;
                    __asm__("lui %0,%%hi(D_8009F8C0)\n\taddiu %0,%0,%%lo(D_8009F8C0)" : "=r"(cbase) : "r"(i));
                    if (!*((s8 *)i + cbase)) {
                        oldsel = D_8009F468->selection[i];
                        func_800787D8((s8)(D_8009F468->selection[i] - 1), -1, (s8)i);
                        if (oldsel != D_8009F468->selection[i]) func_80046C84(0x46, 0);
                    }
                    if (D_8009F468->items[D_8009F468->selection[i]].handler)
                        D_8009F468->items[D_8009F468->selection[i]].handler(4, 0, 0, 0, D_8009F468->selection[i]);
                }
                if (down[i]) {
                    u32 cbase;
                    __asm__("lui %0,%%hi(D_8009F8C0)\n\taddiu %0,%0,%%lo(D_8009F8C0)" : "=r"(cbase) : "r"(i));
                    if (!*((s8 *)i + cbase)) {
                        oldsel = D_8009F468->selection[i];
                        func_800787D8((s8)(D_8009F468->selection[i] + 1), 1, (s8)i);
                        if (oldsel != D_8009F468->selection[i]) func_80046C84(0x46, 0);
                    }
                    if (D_8009F468->items[D_8009F468->selection[i]].handler)
                        D_8009F468->items[D_8009F468->selection[i]].handler(5, 0, 0, 0, D_8009F468->selection[i]);
                }
                if (left[i]) {
                    if (D_8009F468->items[D_8009F468->selection[i]].handler)
                        D_8009F468->items[D_8009F468->selection[i]].handler(2, 0, 0, 0, D_8009F468->selection[i]);
                }
                if (right[i]) {
                    if (D_8009F468->items[D_8009F468->selection[i]].handler)
                        D_8009F468->items[D_8009F468->selection[i]].handler(1, 0, 0, 0, D_8009F468->selection[i]);
                }
            }
        }
        if (D_8009F8BD) {
            for (i = 0; i < count; i++) {
                s32 menu_y;
                D_8009F8B8 = i;
                n = 0;
                for (j = 0; j < D_8009F468->item_count; j++) {
                    if (D_8009F468->items[j].player == i && D_8009F468->items[j].enabled) n++;
                }
                {
                    s32 h = (n << 2) + n;
                    width = h << 1;
                }
                menu_x = D_8009F468->x;
                menu_y = D_8009F468->y;
                if (menu_y + width >= 210) menu_y += 210 - (menu_y + width);
                if (i == 0 && D_8009F468->width) {
                    func_8001C214(0, (menu_x - 6) * 2, (menu_y - 5) * 2, D_8009F468->width * 2, (width + 10) * 2, 1);
                }
                shown = menu_y;
                for (j = 0; j < D_8009F468->item_count; j++) {
                    if (D_8009F468->items[j].player != i) continue;
                    item = &D_8009F468->items[j];
                    if (item->icon != -1) {
                        color = 0x80;
                        if (j == D_8009F468->selection[i] && item->enabled) {
                            if (D_8009F8C0[i]) color = D_8009F3FB + 0x80;
                            else {
                                s32 frame = D_8009FA60;
                                __asm__("lui %0,%%hi(D_8009F3F8)\n\taddiu %0,%0,%%lo(D_8009F3F8)" : "=r"(rgb) : "r"(frame));
                                color = *(((u8 *)((frame >> 4) & 7)) + (u32)rgb) + 0x80;
                            }
                        }
                        x = item->x;
                        y = item->y;
                        func_80082AB0(item->icon, x, y, -1, -1, 10, color, item->flags & 7, item->flags & 0x38);
                    } else if (item->text || item->unk16) {
                        if (j == D_8009F468->selection[i] && item->enabled) {
                            u8 *rgb2;
                            color = 0x11;
                            if (D_8009F8C0[i]) rgb2 = D_8009B3E0;
                            else rgb2 = &D_8009B3D4[((D_8009FA60 >> 4) & 7) * 3];
                            func_800493C0(color, rgb2[0], rgb2[1], rgb2[2]);
                        } else {
                            color = 15;
                            if (item->color != -1) color = item->color;
                        }
                        x = item->x ? item->x : menu_x;
                        y = item->y ? item->y : shown;
                        func_800788F0(item->text, x, y, color, item->unk16, item->flags & 7, item->flags & 0x38, item->unk15);
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
            D_8009F8B8 = i;
            if (D_8009F468->update) D_8009F468->update(i);
        }
        func_800826A0();
        if (D_8009F8C8 == -1) func_80078B98();
        else { func_80078B34(0, 0, 0); func_80078B34(0, 1, 0); }
        if (D_8009F8C8 != -1) {
            D_8009F8BD = 1;
            if (D_8009F8C4) {
                func_80078B34(500, 0, 0);
                func_80078B98();
                func_8001B4A8(1);
                while (D_8009FA3C) { }
                func_80082DDC(D_8009F8C4, 1);
            }
            switch (D_8009F8C8) {
            case 1:
                if (D_8009FA94->type == 2) {
                    if (D_8009FA94->stage == 1) {
                        if (D_8009F8C4) {
                            D_8009B8B8[0] = &D_800A3658;
                            func_80078650(&D_800A3688, 1);
                        } else func_80078650(&D_800A3658, 1);
                    } else func_80078650(&D_800A3658, 1);
                } else {
                    if (D_8009FA94->stage == 1) {
                        if (D_8009F8C4) {
                            D_8009B8B8[0] = &D_800A3628;
                            func_80078650(&D_800A3688, 1);
                        } else func_80078650(&D_800A3628, 1);
                    } else func_80078650(&D_800A3628, 1);
                }
                break;
            case 4:
                if (D_8009FA94->type == 2) func_80078650(&D_800A37D8, 1);
                else func_80078650(&D_800A37A8, 1);
                break;
            case 5:
                if (D_8009F468 == &D_800A3808) {
                    if (D_8009F8C4) {
                        if (D_8009FA94->type == 2) D_8009B8B8[0] = &D_800A37D8;
                        else D_8009B8B8[0] = &D_800A37A8;
                        func_80078650(&D_800A3688, 1);
                    } else {
                        if (D_8009FA94->type == 2) func_80078650(&D_800A37D8, 1);
                        else func_80078650(&D_800A37A8, 1);
                    }
                } else {
                    D_8009B8B8[0] = (struct Menu *)-1;
                    func_80078650(&D_800A3688, 1);
                }
                break;
            }
            D_8009F8C8 = -1;
            D_8009F8C4 = 0;
        }
    }
    D_8009F8D0 = buttons[0];
    D_8009F8D4 = buttons[1];
}
