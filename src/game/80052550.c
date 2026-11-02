#include "game/types.h"

typedef struct {
    u8 unk00[0xAC];
    s8 *unkAC;
    u8 unk00B0[9];
    u8 unkB9;
} Arg0_52550;

typedef struct {
    u8 unk00[0x64];
    s32 unk64;
    s32 unk68;
    s32 unk6C;
    s32 unk70;
    s32 unk74;
    s32 unk78;
} Unk_52550;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec_52550;

extern void func_8001A180(s32 *arg0);
extern s32 func_8001AB00(s32 *arg0, s32 *arg1);
extern void func_8001AC28(s32 *arg0, s32 *arg1, s32 arg2, s32 *arg3);

s32 func_80052550(Arg0_52550 *arg0, Vec_52550 *arg1)
{
    Unk_52550 *unk;
    register s32 *vec __asm__("$17");
    s32 shift;
    s32 ret;

    unk = (Unk_52550 *)(arg0->unkAC + 0xEC);
    if ((unk->unk64 == 0) && (unk->unk68 == 0) && (unk->unk6C == 0)) {
        return 0;
    }
    if (arg0->unkB9 & 1) {
        unk->unk64 = unk->unk64 >> 1;
        unk->unk68 = unk->unk68 >> 1;
        unk->unk6C = unk->unk6C >> 1;
    }
    shift = 1;
    if (arg0->unkAC[0xAD] != 0) {
        goto L_common;
    }
    if (arg0->unkAC[0x147] != 0) {
        vec = &unk->unk64;
        goto L_after_s1;
    }
    shift = 0;
L_common:
    vec = &unk->unk64;
L_after_s1:
    ;
    arg1->x += unk->unk64 >> shift;
    arg1->y += unk->unk68 >> shift;
    arg1->z += unk->unk6C >> shift;
    func_8001A180(vec);
    ret = func_8001AB00(vec, &unk->unk70);
    func_8001AC28(&unk->unk70, vec, -ret, &unk->unk70);
    unk->unk70 -= unk->unk70 / 8;
    unk->unk74 -= unk->unk74 / 8;
    if (unk->unk74 >= 0x101) {
        unk->unk74 = 0;
    }
    unk->unk78 -= unk->unk78 / 8;
    unk->unk6C = 0;
    unk->unk68 = 0;
    unk->unk64 = 0;
    return 1;
}
