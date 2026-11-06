#include "game/types.h"

typedef struct Func80060238Inner {
    u8 pad00[1];
    u8 unk01;
    u8 pad02[4];
    u16 unk06;
    s16 unk08;
    s16 unk0A;
} Func80060238Inner;

typedef struct Func80060238Obj18 {
    u8 pad00[8];
    s32 unk08;
} Func80060238Obj18;

typedef struct Func80060238Sub9C {
    u8 unk00;
} Func80060238Sub9C;

typedef struct Func80060238Arg2 {
    u8 pad00[0xC];
    u8 unk0C;
    u8 pad0D[0xF];
    s16 unk1C;
    u8 pad1E[6];
    u32 unk24;
    u8 pad28[0x74];
    Func80060238Sub9C *unk9C;
} Func80060238Arg2;

typedef struct Func80060238Obj {
    Func80060238Inner *unk00;
    u16 flags04;
    u8 unk06;
    u8 pad07[3];
    s16 unk0A;
    u8 pad0C[4];
    s32 unk10;
    u8 pad14[2];
    u16 unk16;
    Func80060238Obj18 *unk18;
    s32 unk1C;
    s16 unk20;
    s8 unk22;
    u8 unk23;
    u16 unk24;
    s16 unk26;
} Func80060238Obj;

extern s32 D_800A8F90[];

extern void func_8005FFB8(Func80060238Obj *arg0, Func80060238Arg2 *arg1);
extern s32 func_80019668(void);
extern s32 func_8004E5F0(Func80060238Arg2 *arg0, Func80060238Obj18 *arg1, s32 arg2, s32 arg3);
extern void func_80069D68(Func80060238Sub9C *arg0, s32 arg1, s32 arg2, Func80060238Arg2 *arg3);
extern s32 func_800600E8(Func80060238Obj *arg0, s32 arg1, Func80060238Arg2 *arg2);
extern s32 func_8005F6E8(Func80060238Obj *arg0, Func80060238Arg2 *arg1, Func80060238Arg2 *arg2, s32 arg3);

s32 func_80060238(Func80060238Obj *arg0, s32 arg1, Func80060238Arg2 *arg2, Func80060238Arg2 *arg3, u8 arg4)
{
    Func80060238Inner *inner;
    Func80060238Obj18 *obj18;
    Func80060238Arg2 *call_arg2;
    register u32 bytearg __asm__("$22");
    s32 s0;

    bytearg = arg4;

    if (D_800A8F90[1] - D_800A8F90[0] < 10) {
        return 0;
    }
    if (arg0->unk00 == 0) {
        goto L60600;
    }
    if (arg3 != 0 && arg3->unk0C != 0) {
        func_8005FFB8(arg0, arg3);
    }
    if (arg2->unk0C != 0) {
        func_8005FFB8(arg0, arg2);
    }
    inner = arg0->unk00;
    if (inner->unk01 == 0) {
        goto L60600;
    }
    if (arg0->flags04 & 0x100) {
        goto L60600;
    }
    if (arg0->unk20 == 0) {
        goto L60600;
    }
    if (arg0->unk22 != 0) {
        goto L604B0;
    }

    if (arg0->unk26 > 0) {
        arg0->unk24 += arg1;
        if ((s16)arg0->unk24 < inner->unk0A) {
            goto L60468;
        }
        if (arg0->flags04 & 0x20) {
            arg0->unk23 = 1 - arg0->unk23;
        }
        arg0->unk26--;
        s0 = inner->unk0A;
        if (s0 < 0) {
            s0 = -s0;
            s0 = (s0 >> 1) + (func_80019668() % (u32)s0);
        }
        arg0->unk22 = 1;
        arg0->unk24 -= s0;
        goto L60468;
    }

    arg0->unk24 += arg1;
    if ((s16)arg0->unk24 < inner->unk08) {
        goto L60468;
    }
    s0 = inner->unk08;
    if (s0 < 0) {
        s0 = -s0;
        s0 = (s0 >> 1) + (func_80019668() % (u32)s0);
    }
    arg0->unk24 -= s0;
    if (inner->unk0A < (s16)arg0->unk24) {
        arg0->unk24 = inner->unk0A;
    }
    arg0->unk26 = inner->unk06;

L60468:
    if (arg0->unk22 == 0) {
        if (!(arg0->flags04 & 0x4000)) {
            goto L60600;
        }
        arg0->unk16 -= arg1;
        if ((s16)arg0->unk16 < 0) {
            arg0->unk16 = 0;
        }
        goto L60600;
    }

L604B0:
    if (arg0->unk06 == 0) {
        goto L605EC;
    }
    obj18 = arg0->unk18;
    if (obj18 == 0) {
        goto L60584;
    }
    if (obj18->unk08 != arg0->unk1C) {
        arg0->unk18 = 0;
    }
    if (arg0->unk18 != 0) {
        if (func_8004E5F0(arg2, arg0->unk18, arg0->unk0A << 4, arg0->unk10) < 0) {
            arg0->unk18 = 0;
        }
    }
    if (obj18 == 0) {
        goto L60584;
    }
    if (arg0->unk18 != 0) {
        goto L60594;
    }
    if (!(arg0->flags04 & 0x8000)) {
        goto L60584;
    }
    if (arg2->unk9C == 0) {
        goto L60584;
    }
    if (arg2->unk9C->unk00 == 0) {
        goto L60584;
    }
    if (arg2->unk24 & 0x100000) {
        goto L60584;
    }
    func_80069D68(arg2->unk9C, arg2->unk1C < 0, 0, arg2);

L60584:
    if (arg0->unk18 == 0) {
        goto L605A8;
    }
L60594:
    if (!(arg0->flags04 & 0x4000)) {
        goto L605EC;
    }
L605A8:
    if ((u8)func_800600E8(arg0, arg1, arg2) != 0) {
        goto L605EC;
    }
    if (!(arg0->flags04 & 0x4000)) {
        goto L60600;
    }
    if (arg0->unk18 != 0) {
        goto L605EC;
    }
L60600:
    return 0;

L605EC:
    return (u8)func_8005F6E8(arg0, arg2, arg3, bytearg);
}
