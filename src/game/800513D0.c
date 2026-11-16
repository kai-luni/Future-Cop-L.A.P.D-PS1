// Categories: Gameplay > Path Movement | Physics > Motion
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

/**
 * @brief Integrates a 3D position along a looping table of piecewise-linear segments.
 *
 * Advances arg1 along arg0->unk10 by up to arg2 units per iteration using func_8001AB78,
 * stepping through 3-word (z, x, length) records at arg0->unk20 when each segment
 * finishes, wrapping to arg0->unk1C at unk28 and snapping to unk2C when within 0x2000.
 *
 * @param arg0 Path state (Unk513D0: active flag +0x00, segment len/pos +0x08/+0x0C, dir +0x10, base/cur +0x1C/+0x20, idx/count +0x24/+0x28, end +0x2C).
 * @param arg1 3D position vector (Vec513D0) updated in place.
 * @param arg2 Remaining motion budget to advance.
 * @param arg3 Optional counter incremented on each segment transition, or null.
 * @return 1 if the path wrapped past segment unk28 during this call, else 0.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Returns 0 if arg0->unk00 == 0; loops while arg2 > 0 clamping each step to (unk08 - unk0C), advancing segment records and wrapping at unk24 == unk28.
 *
 * @par Calls
 * func_8001AB78 - scales copied segment vector by step amount; func_8001A13C - checks XZ distance to unk2C on wrap.
 *
 * @par Side effects
 * Updates *arg1, arg0 progress/segment fields (unk08, unk0C, unk10, unk20, unk24), and *arg3 if non-null.
 *
 * @par Callers
 * Confirmed: none (unreferenced in asm/us/main.s and marked orphan/unreachable in analysis/analysis.json).
 */
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
