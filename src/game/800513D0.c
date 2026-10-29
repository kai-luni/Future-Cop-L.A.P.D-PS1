#include "game/types.h"

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec513D0;

typedef struct {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    Vec513D0 unk10;
    s32 *unk1C;
    s32 *unk20;
    s32 unk24;
    s32 unk28;
    Vec513D0 unk2C;
} Unk513D0;

extern void func_8001AB78(s32 length, s32 *vector);
extern s32 func_8001A13C(s32 *a, s32 *b);

s32 func_800513D0(Unk513D0 *arg0, Vec513D0 *arg1, s32 arg2, s32 *arg3)
{
    s32 done;
    s32 amount;
    Vec513D0 local;

    done = 0;
    if (arg0->unk00 == 0) {
        return done;
    }

    while (arg2 > 0) {
        if (arg0->unk08 < arg2 + arg0->unk0C) {
            amount = arg0->unk08 - arg0->unk0C;
            arg2 -= amount;
        } else {
            amount = arg2;
            arg2 = 0;
        }
        arg0->unk0C += amount;

        local = arg0->unk10;
        func_8001AB78(amount, (s32 *)&local);

        arg1->x += local.x;
        arg1->y += local.y;
        arg1->z += local.z;

        if (arg2 <= 0) {
            break;
        }

        arg0->unk10.z = arg0->unk20[0];
        arg0->unk10.x = arg0->unk20[1];
        arg0->unk08 = arg0->unk20[2];
        arg0->unk0C = 0;

        arg0->unk24++;
        if (arg3 != 0) {
            (*arg3)++;
        }
        arg0->unk20 += 3;

        if (arg0->unk24 == arg0->unk28) {
            done = 1;
            arg0->unk24 = 0;
            arg0->unk20 = arg0->unk1C;
            if (func_8001A13C((s32 *)arg1, (s32 *)&arg0->unk2C) < 0x2000) {
                *arg1 = arg0->unk2C;
            }
        }
    }

    return done;
}
