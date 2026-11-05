#include "game/types.h"

struct State27F80 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
};

struct MenuText27F80 {
    u8 *text[0x40];
};

extern struct State27F80 *D_8009F68C;
extern struct MenuText27F80 *D_8009F2C0;
extern u8 D_8009F2FC[];
extern u8 D_80010DCC[];

extern s32 func_80059E9C(void *player, s32 arg1);
extern s32 func_80049340(u8 *text);
extern void func_80049100(u8 *text, s32 x, s32 y, s32 color);
extern void func_8008CD30(u8 *dest, u8 *fmt, ...);

void func_80027F80(void *player, u8 *text, s32 x, s32 color)
{
    s32 c;
    u8 *str;
    s32 v;
    s32 t;
    s32 y;
    s32 i;
    u8 buf[0x10];

    y = 0x21;
    i = 0;
    c = text[0];
    if (c == 0x1B) {
        return;
    }
    do {
        if (c == 0x1A) {
            y -= 4;
        } else if (c != 0x19) {
            if (c >= 0x40) {
                c -= 0x40;
                if (D_8009F68C->unk0 == 0) {
                    t = func_80059E9C(player, c);
                    ((s32 *)D_8009F68C)[D_8009F68C->unk4] = t;
                } else {
                    t = ((s32 *)D_8009F68C)[D_8009F68C->unk4];
                }
                D_8009F68C->unk4 = D_8009F68C->unk4 + 1;
                if (c >= 0xF) {
                    t += 0x96;
                    t /= 300;
                    func_8008CD30(buf, D_80010DCC, t / 60, t % 60);
                } else {
                    func_8008CD30(buf, D_8009F2FC, t);
                }
                str = buf;
            } else {
                str = D_8009F2C0->text[c];
            }
            v = func_80049340(str);
            if (v < 0) {
                v += 3;
            }
            func_80049100(str, x - (v >> 2), y, color & 0xFFFF);
        }
        i++;
        c = text[i];
        y += 9;
    } while (c != 0x1B);
}
