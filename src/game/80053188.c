#include "game/types.h"

struct Unk53188Sub;

struct Unk53188 {
    u8 pad00[0x50];
    s32 unk50;
    u8 pad54[4];
    s32 unk58;
    u8 pad5C[0x50];
    struct Unk53188Sub *unkAC;
};

struct Unk53188Sub {
    u8 pad00[0xAD];
    s8 unkAD;
    u8 padAE[0x13];
    s8 unkC1;
    u8 padC2[0x1C];
    u8 unkDE;
    u8 padDF[0x67];
    u8 unk146;
    u8 unk147;
    u8 pad148[0x1B5];
    u8 unk2FD;
    u8 pad2FE[0x28];
    s16 unk326;
};

struct Unk53188Arg {
    s32 unk0;
    s16 unk4;
    u8 unk6;
    u8 pad7;
    s32 unk8;
};

extern u8 *D_8009FA94;
extern u8 D_8009FA8C[];
extern s8 D_8009ACF4[][10];

extern void func_8005908C(s32 arg0, struct Unk53188 *arg1,
                           struct Unk53188Arg *arg2, s32 arg3);

void func_80053188(struct Unk53188 *arg0, u32 arg1)
{
    register struct Unk53188Sub *p __asm__("$4");
    struct Unk53188Arg arg;
    s32 state;
    s32 value;
    register s32 slot __asm__("$3");
    register s8 *tablep __asm__("$3");
    register s32 index __asm__("$2");
    u8 *ptr;

    state = 0;

    if ((arg1 - 0x20) < 0x10) {
        switch (arg1) {
        case 0x20:
            arg0->unk58 += 0x200;
            break;
        case 0x21:
            arg0->unk58 += 0x400;
            break;
        case 0x22:
            arg0->unk58 -= 0x200;
            break;
        case 0x23:
            arg0->unk58 -= 0x400;
            break;
        case 0x24:
            arg0->unk50 += 0x200;
            break;
        case 0x25:
            arg0->unk50 += 0x400;
            break;
        case 0x26:
            arg0->unk50 -= 0x200;
            break;
        case 0x27:
            arg0->unk50 -= 0x400;
            break;
        case 0x2B:
            arg0->unk50 -= 0x92;
            break;
        case 0x2A:
            arg0->unk50 += 0x92;
            break;
        case 0x28:
            arg0->unk58 += 0x92;
            break;
        case 0x29:
            arg0->unk58 -= 0x92;
            break;
        case 0x2F:
            arg0->unk50 -= 0x124;
            break;
        case 0x2E:
            arg0->unk50 += 0x124;
            break;
        case 0x2C:
            arg0->unk58 += 0x124;
            break;
        case 0x2D:
            arg0->unk58 -= 0x124;
            break;
        }
    } else {
        arg.unk4 = 0;

        if (arg1 < 0x10) {
            switch (arg1) {
            case 0:
                break;
            case 1:
                arg0->unkAC->unk146 = 1;
                if (arg0->unkAC->unkC1 != 0) {
                    ptr = D_8009FA94 + 0x98;
                } else {
                    ptr = D_8009FA94 + 0x8;
                }
                if (*(s32 *)ptr != 0) {
                    goto case1_body;
                }
                p = arg0->unkAC;
                if (p->unkAD != 0) {
                    break;
                }
                __asm__("" : "+r"(p));
                index = p->unkC1;
                __asm__("" : "+r"(index));
                if (D_8009FA8C[index] != 0) {
                    goto case1_body;
                }
                p->unkDE = 1;
                arg0->unkAC->unk326 = 0;
                break;
            case1_body:
                {
                    register struct Unk53188Sub *q __asm__("$3");
                    q = arg0->unkAC;
                    if (q->unkAD == 0) {
                        q->unk2FD = 1;
                    }
                }
                break;
            case 2:
                {
                register struct Unk53188Sub *r __asm__("$2");
                r = arg0->unkAC;
                __asm__("" : "+r"(r));
                index = r->unkC1;
                __asm__("" : "+r"(index));
                if (D_8009FA8C[index] == 0) {
                    arg.unk4 = 10000;
                }
                }
                break;
            case 3:
                state = 2;
                break;
            case 4:
                state = 1;
                break;
            case 5:
                state = 3;
                break;
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 0xE:
            case 0xF:
                p = arg0->unkAC;
                __asm__("" : "+r"(p) : : "$2");
                slot = p->unkC1;
                __asm__("" : "+r"(slot));
                if (D_8009FA8C[slot] != 0) {
                    break;
                }
                if (arg1 == 0xE && slot != 0) {
                    break;
                }
                if (arg1 == 0xF && slot != 1) {
                    break;
                }
                value = D_8009ACF4[p->unkAD][arg1 - 6];
                if (value < 0) {
                    value = 10000;
                }
                arg.unk4 = value;
                break;
            }
        }

        if (arg.unk4 != 0) {
            index = arg1 - 0x64;
            __asm__("" : "+r"(index));
            arg.unk0 = index;
            __asm__("" : : "m"(arg.unk0) : "$4");
            index = 1;
            __asm__("" : "+r"(index) : : "$4");
            arg.unk6 = index;
            arg.unk8 = 0;
            func_8005908C(0xE, arg0, &arg, 0);
        }
    }

    arg0->unkAC->unk147 = state;
}
