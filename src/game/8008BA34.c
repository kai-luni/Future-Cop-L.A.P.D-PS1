#include "game/types.h"

typedef struct UnkBA34_Item {
    u8 unk00[0x0C];
    u8 *unk0C;
    u8 unk10[0xF0 - 0x10];
} UnkBA34_Item;

typedef struct UnkBA34_Arg {
    u8 unk00[0x0C];
    u8 *unk0C;
    u8 unk10[0x36 - 0x10];
    u8 unk36;
    u8 unk37[0x3C - 0x37];
    u8 *unk3C;
    u8 unk40[0x44 - 0x40];
    u8 unk44;
} UnkBA34_Arg;

extern void (*D_8009C728)(s32);
extern u8 (*D_8009C730)(UnkBA34_Arg *, s32);
extern void (*D_8009C734)();
extern void (*D_8009C748)(u8 *);
extern void (*D_8009C74C)(u8 *);
extern UnkBA34_Item *D_8009C75C;
extern s32 D_8009C768;
extern s32 D_8009C774;
extern s32 D_8009C780[2];
extern s32 D_8009C7A0;
extern volatile u8 *D_8009C7A4;

extern s32 func_8008A9A8(UnkBA34_Arg *, u8);
extern s32 func_8008AB80(UnkBA34_Arg *, u8);
extern s32 func_8008ADB4(void);
extern void func_8008AE44(void);
extern void func_8008C890(s32);

s32 func_8008BA34(UnkBA34_Arg *arg0)
{
    s32 i;
    s32 off;
    s32 s4;
    s32 idx;
    u8 *ptr;
    s32 ret;

    D_8009C734();

    s4 = 0;
    if (D_8009C774 != 0 && ((*arg0->unk3C >> 4) == 8)) {
        s4 = (arg0->unk36 == 0);
    }

    if (s4 != 0) {
        for (i = -1, off = -0xF0; i < 4; i++, off += 0xF0) {
            if (--D_8009C7A0 <= 0) {
                break;
            }
            if (i >= 0) {
                D_8009C734(arg0->unk0C + off);
            }
            ret = func_8008AB80(arg0, D_8009C730(arg0, 1));
            if (ret < 0) {
                return ret;
            }
            func_8008C890(0x3C);
            if (func_8008ADB4() == 0) {
                return -3;
            }
        }
    }

    idx = (D_8009C768 == 0);
    while (D_8009C7A0 >= 2) {
        if (D_8009C780[idx] < 0) {
            break;
        }
        if (D_8009C780[idx] > 0) {
            ptr = D_8009C75C[idx].unk0C + D_8009C780[idx] * 0xF0 - 0xF0;
            D_8009C748(ptr);
        }
        switch (D_8009C780[idx]) {
        case 4:
            D_8009C780[idx] = 3;
            break;
        case 3:
            D_8009C748(ptr - 0xF0);
            D_8009C780[idx] = 1;
            break;
        case 0:
        case 1:
            ptr = (u8 *)&D_8009C75C[idx];
            D_8009C748(ptr);
            D_8009C74C(ptr);
            D_8009C780[idx] = -1;
            break;
        }
        ret = func_8008A9A8(arg0, D_8009C730(arg0, s4));
        if (ret < 0) {
            return ret;
        }
        func_8008C890(0x3C);
        if (func_8008ADB4() == 0) {
            return -3;
        }
        --D_8009C7A0;
    }

    while (--D_8009C7A0 > 0) {
        ret = func_8008A9A8(arg0, D_8009C730(arg0, s4));
        if (ret < 0) {
            return ret;
        }
        func_8008C890(0x3C);
        if (func_8008ADB4() == 0) {
            return -3;
        }
    }

    func_8008AE44();
    arg0->unk3C[arg0->unk44++] = *D_8009C7A4;
    D_8009C728(0);
    return 0;
}
