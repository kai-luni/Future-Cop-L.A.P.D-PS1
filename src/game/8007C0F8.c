#include "game/types.h"

struct MenuTable7C0F8 {
    u16 sprite;
    u8 pad0[2];
    u16 *items;
    u16 count;
    u8 pad1[2];
};

struct MenuItem7C0F8 {
    s8 x;
    s8 y;
    s8 x2;
    s8 y2;
    s8 amount;
    s8 y3;
    s8 flag;
    u8 pad[5];
};

struct Work7C0F8 {
    u8 text[0x10];
    u8 *selData;
    struct MenuTable7C0F8 *table;
    u16 mode;
    u8 pad[6];
};

extern u8 D_8009F902[];
extern s8 D_8009F900[];
extern u8 D_8009C018[];
extern u8 D_8009BFD4[];
extern u8 D_8009BEF4[];
extern u16 D_8009BF9C[][2];
extern u8 D_8009F484[];
extern s8 D_800A374D[];
extern u8 D_800A6FE0[];

extern void func_800788F0(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7);
extern void func_8008CD30(u8 *dst, u8 *fmt, s32 value);
extern s32 func_8001EF34(s32 port);
extern void func_80082AB0(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8);
extern void func_80082EF0(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7);

#define MIN(a, b) (((((a) - (b)) >> 31) & ((a) - (b))) + (b))
#define MAX(a, b) ((a) - ((((a) - (b)) >> 31) & ((a) - (b))))
#define ABS(a) ((((a) >> 31) + (a)) ^ ((a) >> 31))

void func_8007C0F8(s32 slot)
{
    u8 text[0x10];
    u8 *selData;
    struct MenuTable7C0F8 *table;
    u16 mode;
    struct MenuItem7C0F8 *item;
    s32 i;
    register s32 off __asm__("$22");
    s32 center_x;
    s32 center_y;
    s32 layer;
    s32 green;
    s32 blue;
    s32 yellow;
    u32 choice;
    s32 sel;
    u16 sprite;
    s32 dy;
    s32 port;
    s32 flag;
    s32 caption_y;
    s32 red;
    s32 c2 = 2;
    s32 c4 = 4;
    s8 *selPtr;
    register s8 *selBase __asm__("$2");
    register u8 *bufArg __asm__("$4");
    register u8 *fmtHi __asm__("$5");

    choice = D_8009F902[slot];
    table = (struct MenuTable7C0F8 *)(D_8009C018 + choice * 12);
    func_800788F0((s32)(D_800A6FE0 + slot * 9), 0x28, 0x34, 0xF, 0, 1, 8, c2);
    func_800788F0(0, 0xA0, 0x34, 0xF, table->sprite, c4, 8, c2);
    func_800788F0(0, 0x10E, 0x34, 0xF, 0x12F2, 2, 8, c2);
    bufArg = text;
    fmtHi = (u8 *)0x800A0000;
    __asm__("lui\t%0,%%hi(D_8009F900)\n\taddiu\t%0,%0,%%lo(D_8009F900)" : "=r"(selBase), "+r"(fmtHi) : "r"(slot), "r"(bufArg));
    selPtr = (s8 *)(slot + (s32)selBase);
    func_8008CD30(bufArg, fmtHi - 2940, *selPtr + 1);
    func_800788F0((s32)text, 0x10E, 0x34, 0xF, 0, 1, 8, c2);
    mode = 0x48;
    port = func_8001EF34(D_800A374D[0] == 3);
    center_x = 0xA0;
    center_y = 0x69;
    if (port == 7) {
        mode = 0x49;
    }
    func_80082AB0(mode, 0xA0, 0x69, -1, -1, 0x14, 0x80, c4, 0x20);

    i = 0;
    layer = 10;
    red = 0x60;
    green = 0xC0;
    blue = 0xFF;
    yellow = 0x64;

    sel = *selPtr;
    off = i;
    selData = D_8009BFD4 + sel * 14;

    for (; i < 14; i++) {
        item = (struct MenuItem7C0F8 *)(D_8009BEF4 + off);
        if (i < 7) {
            flag = 2;
            dy = -2;
        } else {
            flag = 1;
            dy = 2;
        }

        sprite = D_8009BF9C[selData[i]][D_8009F902[slot]];
        func_800788F0(0, center_x + item->x + dy, center_y + item->y, 0x10,
                      sprite, flag, 0x20, c2);

        if (mode == 0x49 && item->amount != 0) {
            if (item->flag == 1) {
                func_80082EF0(center_x + MIN(item->x, item->amount),
                              center_y + item->y,
                              ABS(item->x - item->amount) + 1,
                              1, layer, red, green, blue);
                func_80082EF0(center_x + item->amount,
                              center_y + MIN(item->y3, item->y),
                              1,
                              ABS(item->y3 - item->y) + 1,
                              layer, red, green, blue);
            } else if (i == 4) {
                func_80082EF0(center_x + MIN(item->x2, item->amount),
                              center_y + item->y2,
                              1,
                              ABS(item->y3 - item->y2) + 1,
                              layer, blue, blue, yellow);
                func_80082EF0(center_x + MIN(item->x2, item->amount),
                              center_y + MAX(item->y2, item->y3),
                              ABS(item->amount - item->x2) + 1,
                              1, layer, blue, blue, yellow);
            } else {
                func_80082EF0(center_x + MIN(item->x, item->amount),
                              center_y + item->y,
                              ABS(item->x - item->amount) + 1,
                              1, layer, blue, blue, yellow);
                func_80082EF0(center_x + item->amount,
                              center_y + MIN(item->y3, item->y),
                              1,
                              ABS(item->y3 - item->y) + 1,
                              layer, blue, blue, yellow);
            }
        }
        if (mode != 0x49 || item->flag == 0) {
            func_80082EF0(center_x + MIN(item->x, item->x2),
                          center_y + item->y,
                          ABS(item->x - item->x2) + 1,
                          1, layer, red, green, blue);
            func_80082EF0(center_x + item->x2,
                          center_y + MIN(item->y2, item->y),
                          1,
                          ABS(item->y2 - item->y) + 1,
                          layer, red, green, blue);
        }
        off += 12;
    }

    for (i = 0; i < table->count; i++) {
        func_800788F0(0, center_x, center_y + 0x2A + i * 10, 0xF,
                      table->items[i], 4, 8, 2);
    }
}
