#include "game/types.h"

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec4B310;

typedef struct {
    u8 unk00[0x66];
    s16 unk66;
    s16 unk68;
    s16 unk6A;
    s16 unk6C;
    s16 unk6E;
    s16 unk70;
    s16 unk72;
    s16 unk74;
    s16 unk76;
    s16 unk78;
    u8 unk7A;
    u8 unk7B;
    s8 unk7C;
} Obj4B310;

extern s32 func_80019668(void);
extern void func_8001A180(Vec4B310 *arg);

void func_8004B310(Obj4B310 *arg0, Vec4B310 *arg1, Vec4B310 *arg2, s32 arg3,
                   s16 *arg4, s16 *arg5, s32 *arg6, Vec4B310 *arg7)
{
    s32 s0;

    s0 = arg0->unk78 - arg0->unk76;
    if (s0 > 0) {
        *arg4 = arg0->unk76 + (((u32)(s0 * (func_80019668() & 0xFFF))) >> 12);
        *arg5 = arg0->unk76 + (((u32)(s0 * (func_80019668() & 0xFFF))) >> 12);
    } else {
        *arg4 = arg0->unk78;
        *arg5 = arg0->unk78;
    }

    *arg6 = ((func_80019668() & 0x1F) + 0xA0) << 3;
    *arg6 = -((*arg6 * arg0->unk7C) / 100) * 16;

    func_8001A180(arg1);

    s0 = arg0->unk68 - arg0->unk66;
    if (s0 > 0) {
        s0 += arg0->unk66 + (((u32)(s0 * (func_80019668() & 0xFFF))) >> 12);
    } else {
        s0 = arg0->unk68;
    }

    arg7->x = ((arg1->x >> 4) * s0) >> 8;
    arg7->y = ((arg1->y >> 4) * s0) >> 8;
    arg7->z = ((arg1->z >> 4) * s0) >> 8;

    if (arg7->y < 0) {
        arg7->y = -arg7->y;
    }

    s0 = arg0->unk6C - arg0->unk6A;
    if (s0 > 0) {
        s0 = arg0->unk6A + (((u32)(s0 * (func_80019668() & 0xFFF))) >> 12);
    } else {
        s0 = arg0->unk6C;
    }

    if (s0 != 0) {
        arg7->x += (arg0->unk70 * s0) >> 8;
        arg7->y += (arg0->unk72 * s0) >> 8;
        arg7->z += (arg0->unk74 * s0) >> 8;
    }

    if (arg3 & 0xFF) {
        arg7->x += arg2->x;
        arg7->y += arg2->y;
        arg7->z += arg2->z;
    }

    if (arg0->unk7B & 4) {
        arg7->y = 0;
    }
}
