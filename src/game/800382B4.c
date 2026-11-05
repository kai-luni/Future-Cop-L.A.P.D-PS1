#include "game/types.h"

typedef struct Unk382B4Sub {
    u8 pad00[0x10];
    u32 unk10;
} Unk382B4Sub;

typedef struct Unk382B4CC {
    u8 pad00;
    u8 unk01;
} Unk382B4CC;

typedef struct Unk382B4 {
    u8 pad00[0xC];
    u8 unk0C[8];
    u8 pad14[2];
    u8 unk16;
    u8 pad17[2];
    u8 unk19;
    u8 pad1A[2];
    s16 unk1C;
    u8 pad1E[2];
    u8 unk20;
    u8 pad21[3];
    u32 unk24;
    Unk382B4Sub *unk28;
    u8 pad2C[0x58];
    s32 unk84;
    u8 pad88[0x18];
    u32 unkA0;
    u8 padA4[0x24];
    u8 unkC8[4];
    Unk382B4CC *unkCC;
    u8 padD0[4];
    s16 unkD4;
    u8 padD6[6];
    u8 *unkDC;
    u8 padE0[0x22];
    s16 unk102;
    u8 pad104[0x14];
    u32 unk118;
    u8 pad11C[2];
    s16 unk11E;
    s16 unk120;
    u8 pad122[8];
    s16 unk12A;
    u8 pad12C[0x2C];
    u8 unk158;
    u8 unk159;
    u8 unk15A;
} Unk382B4;

extern void *jtbl_800113DC[16];

extern s32 func_8002AD58(s32 arg0, Unk382B4 *arg1, s32 arg2, s32 arg3);
extern void func_800378BC(s32 arg0, Unk382B4 *arg1, s32 arg2, s32 arg3);
extern void func_800379D4(s32 arg0, Unk382B4 *arg1, s32 arg2, s32 arg3);
extern void func_800381D4(Unk382B4 *arg1);
extern void func_800448D8(void *arg0, u8 arg1, s32 arg2, s32 arg3);
extern void func_8003B898(u8 arg0, Unk382B4 *arg1, s32 arg2, s32 arg3, s32 arg4);
extern void func_80037B28(s32 arg0, Unk382B4 *arg1, s32 arg2);
extern void func_8002A5B4(Unk382B4 *arg0, s32 arg1);
extern s32 func_80044B34(Unk382B4 *arg0, void *arg1, s32 arg2, s32 arg3);
extern void func_800175F8(void *arg0, s32 arg1);
extern void func_80060238(void *arg0, s32 arg1, Unk382B4 *arg2, s32 arg3, s32 arg4);
extern void func_8003802C(s32 arg0, Unk382B4 *arg1, s32 arg2);
extern s32 func_8002ACB4(Unk382B4 *arg0, u8 arg1);
extern void func_80063194(s32 key);

register s32 zero __asm__("$0");

s32 func_800382B4(s32 arg0, Unk382B4 *arg1, s32 arg2, s32 arg3)
{
    register s32 msgidx __asm__("$3");
    s32 scale;
    s32 i;
    s32 temp;
    s16 idx;
    u8 v;
    register u32 flag __asm__("$3");
    register s32 one __asm__("$2");
    register s32 multiplicand __asm__("$2");
    u8 *base;
    u8 *entry;
    u8 count;
    static void *const possible_targets[] = {
        &&L80038310, &&L80038318, &&L8003832C, &&L80038334,
        &&L8003836C, &&L8003839C, &&L800383E0, &&L80038498,
        &&L800384B0, &&L800385BC, &&L800385FC, &&L80038614,
        &&L80038650, &&L800386D8, &&L8003870C, &&L80038718
    };

    msgidx = arg0 - 1;
    if ((u32)msgidx < 16) {
        goto *jtbl_800113DC[msgidx];
    }
    goto L80038718;

L80038310:
    return 0x15C;

L80038318:
    func_800378BC(arg0, arg1, arg2, arg3);

L8003832C:
    return zero;

L80038334:
    if (arg1->unk16 == 5 && arg1->unk15A == 1) {
        return 0;
    }
    func_8002AD58(arg0, arg1, arg2, arg3);
    goto L8003839C;

L8003836C:
    if (arg1->unk16 == 5 && arg1->unk15A == 1) {
        return 0;
    }
    func_800379D4(arg0, arg1, arg2, arg3);

L8003839C:
    if ((arg1->unk118 & 0x2000) != 0) {
        if (arg1->unk1C < 0) {
            if (arg1->unk102 > 0) {
                func_800381D4(arg1);
            }
        }
    }
    return 0;

L800383E0: {
    register s32 state __asm__("$3");
    register s32 one __asm__("$2");
    if (arg1->unk16 != 5) {
        goto L8003871C;
    }
    if (arg1->unk28->unk10 == 0) {
        goto L8003871C;
    }
    if (arg1->unk158 == 0xFF) {
        goto L80038720;
    }
    state = arg1->unk15A;
    one = 1;
    if (state == 0) {
        register s32 six __asm__("$6");
        register s32 two __asm__("$2");
        register s32 seven __asm__("$7");
        register u8 *a0 __asm__("$4");
        register s32 five __asm__("$5");
        a0 = arg1->unkC8;
        __asm__ volatile ("" : "+r" (a0), "+r" (one));
        six = 1;
        __asm__ volatile ("" : "+r" (six));
        seven = six;
        __asm__ volatile ("" : "+r" (seven));
        five = arg1->unk158;
        __asm__ volatile ("" : : "r" (five));
        two = six;
        __asm__ volatile ("" : "+r" (two), "+r" (seven));
        arg1->unk1C = two;
        arg1->unk15A = two;
        arg1->unk118 |= 0x40;
        arg1->unk24 |= 1;
        func_800448D8((void *)a0, five, six, seven);
        func_8003B898(arg1->unk20, arg1, 0, (arg1->unk19 >> 3) & 0x1E, 0);
        return 0;
    }
    if (state != one) {
        goto L8003871C;
    }
    arg1->unk1C = state;
    goto L8003832C;
}

L80038498:
    func_80037B28(arg0, arg1, arg2);
    return 0;

L800384B0:
    if (arg1->unk28->unk10 == 0) {
        goto L800385BC;
    }
    scale = arg2;
    flag = arg1->unkCC->unk01;
    one = 1;
    if (flag != one) {
        multiplicand = arg1->unk84;
        scale = multiplicand * arg2;
    }
    if (arg1->unk16 == 5) {
        flag = arg1->unk15A;
        one = 1;
        if (flag == one) {
            for (i = 0; i < 4; ) {
                v = arg1->unk0C[i * 2];
                if (v == 0) {
                    break;
                }
                if ((u8)(v - 0x32) < 3) {
                    func_8002A5B4(arg1, i);
                } else {
                    i++;
                }
            }
        }
    }
    temp = func_80044B34(arg1, arg1->unkC8, scale, 0);
    func_800175F8((void *)&arg1->unk28, arg1->unkD4);
    temp >>= 1;
    if (arg1->unk16 == 5 && temp == 3 && arg1->unk15A == 1) {
        arg1->unk1C = -1;
        arg1->unk15A = 2;
        return 0;
    }

L800385BC:
    if (arg1->unkA0 != 0) {
        func_80060238(&arg1->unkA0, arg2, arg1, 0, 1);
    }
    func_8003802C(arg0, arg1, arg2);
    return 0;

L800385FC:
    if (arg2 != 6) {
        goto L8003871C;
    }
    return arg1->unk11E;

L80038614:
    if (arg1->unk16 != 5) {
        goto L8003871C;
    }
    if (arg1->unk28->unk10 == 0) {
        goto L80038720;
    }
    return func_8002ACB4(arg1, arg1->unk159);

L80038650:
    {
        register u8 *b __asm__("$5");
        s32 off;
        register s32 c __asm__("$4");
        register s32 byte __asm__("$2");
        register u8 *p __asm__("$2");
        register s32 tmp __asm__("$2");
        off = arg1->unk120;
        if (off >= 0) {
            b = arg1->unkDC;
            if (off < *(s16 *)(b + 2)) {
                tmp = off * 3;
                off = tmp * 4;
                byte = b[off + 8];
                __asm__ volatile ("" : "+r" (byte));
                c = byte & 0xF;
                if (c > 0) {
                    p = b + (off + 4);
                    p[4] = (p[4] & 0xF0) | (c - 1);
                }
            }
        }
    }
    if ((arg1->unk118 & 0x4000) != 0) {
        func_80063194((s32)arg1);
    }
    goto L8003871C;

L800386D8:
    if (arg1->unk16 != 5) {
        return 1;
    }
    if (arg1->unk28->unk10 == 0) {
        return 1;
    }
    return arg1->unk159;

L8003870C:
    return arg1->unk12A;

L80038718:
L8003871C:
L80038720:
    return func_8002AD58(arg0, arg1, arg2, arg3);
}
