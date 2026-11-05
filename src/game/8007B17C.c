#include "game/types.h"

struct Unk8009FA94 {
    u8 pad00[0x0C];
    s32 unk0C;
    u8 pad10[0x4C];
    s32 unk5C;
};

struct Unk8009F468 {
    s8 unk0;
    s8 unk1;
    u8 pad2[3];
    s8 unk5;
};

extern struct Unk8009F468 *D_8009F468;
extern struct Unk8009FA94 *D_8009FA94;
extern s8 D_8009F8C0[];
extern volatile s32 D_8009FA60;

extern void func_800788F0(u8 *text, s32 x, s32 y, s32 color, s32 textId,
                          s32 alignX, s32 alignY, s32 font);
extern void func_80078B34(s32 arg0, s32 arg1, s32 arg2);

void func_8007B17C(s32 arg0)
{
    s32 b = 0;
    s32 a = 0;

    if (arg0 == 0) {
        struct Unk8009FA94 *g = D_8009FA94;
        s32 y = (g->unk0C == 2) ? 0x37 : 0x41;
        s32 id = (g->unk5C == 1) ? 0x438 : 0x42E;
        func_800788F0(0, 0xA0, y, 0x10, id, 4, 8, 1);
    }

    if (D_8009F468->unk5 == 2) {
        if (arg0 == 1) {
            b = 2;
            switch (D_8009F468->unk1) {
            case 5: a = 1; break;
            case 6: a = 2; break;
            case 4: a = 3; break;
            }
        } else {
            b = 1;
            switch (D_8009F468->unk0) {
            case 2: a = 1; break;
            case 3: a = 2; break;
            case 1: a = 3; break;
            }
        }
    } else {
        switch (D_8009F468->unk0) {
        case 1: a = 1; break;
        case 2: a = 2; break;
        case 0: a = 3; break;
        }
    }

    switch (a) {
    case 1:
        __asm__ volatile("");
        if (D_8009F8C0[arg0] != 0) {
            s32 id;
            if ((D_8009FA60 >> 9) & 1) {
                id = 0x6C2;
            } else if ((D_8009FA60 >> 10) & 1) {
                id = 0x6E5;
            } else {
                id = 0x6D6;
            }
            func_80078B34(id, 0, b);
            func_80078B34(0x6E0, 1, b);
        } else {
            func_80078B34(0x6EA, 0, b);
            func_80078B34(0x44C, 1, b);
        }
        break;
    case 2:
        func_80078B34(0x6B8, 0, b);
        func_80078B34(0x44C, 1, b);
        break;
    case 3:
        func_80078B34(0x6AE, 0, b);
        func_80078B34(0x44C, 1, b);
        break;
    }

    if ((D_8009FA60 >> 9) & 1) {
        func_80078B34(0x10CC, 1, b);
    }
}
