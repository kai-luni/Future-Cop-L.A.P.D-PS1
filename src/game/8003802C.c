// Categories: Physics > Motion | Graphics > Scene Graph | Gameplay > Object Dynamics
#include "game/types.h"

typedef struct UnkSub {
    u8 pad00[0x34];
    u8 *unk34;
} UnkSub;

typedef struct Unk3802C {
    u8 pad00[0x1C];
    s16 unk1C;
    u8 pad1E[0x0A];
    UnkSub *unk28;
    u8 unk2C[0x24];
    u8 unk50[4];
    s32 unk54;
    u8 pad58[4];
    u8 unk5C[0x0C];
    s32 unk68;
    u8 pad6C[4];
    s16 unk70;
    u8 pad72[0x32];
    u16 unkA4;
    u8 padA6[4];
    s16 unkAA;
    s32 unkAC;
    s32 unkB0;
    u16 unkB4;
    u8 padB6[0x26];
    u8 unkDC[0x26];
    s16 unk102;
    u8 pad104[0x14];
    u32 unk118;
} Unk3802C;

extern s32 func_80050D14(void *arg0, s32 arg1, void *arg2);
extern s32 func_80066F78(void *arg0, s32 arg1, s32 arg2, s32 arg3);
extern void func_80037EB4(Unk3802C *arg0);
extern void func_80010750(void *arg0);
extern void func_80010234(void *arg0, void *arg1, s32 arg2);

/**
 * @brief Advances a falling or flying object by one time step, clamps it
 *        against a ground limit and refreshes its transform.
 *
 * @param arg0 Unused in this body.
 * @param arg1 Object record (height unk54, speed unkAC, flags unk118).
 * @param arg2 Time step.
 * @return Nothing.
 *
 * @par Confirmed
 * ret = func_80050D14(&arg1->unkDC, arg2, arg1). When (unk118 >> 30) == 1:
 * unkA4 |= 0x100; unkB0 = (unkB0 + 2*arg2) & 0xFFF, clamped to 1000 when
 * the result lies in [1001, 1001 + 0x816]; unkAC += arg2 >> 1;
 * unkB4 = (unkB4 + arg2*unkAA) & 0xFFF. The ground limit is
 * func_80066F78(&unk50, 0, 0, 0) + (unk68 >> 1). With delta = unkAC*arg2,
 * if unk54 - delta < limit the descent is truncated, unk1C = -1 and
 * unk118 |= 0xC0000000 (landing flags); when ret == 0, unk54 -= delta;
 * ret becomes 2 and unk102 -= delta >> 4.
 * When ret != 0: func_80037EB4(arg1) runs if (ret & 2) or (unk118 & 0x200),
 * then func_80010750(&unk2C) loads the matrix and func_80010234 transforms
 * one vertex from the model data into unk5C.
 *
 * @par Likely role
 * Ballistic or hovering object: unk54 is the height, unkAC the descent
 * speed, the 0xC0000000 bits mark the landed state.
 *
 * @par Callers
 * func_8002B784, func_800382B4.
 */
void func_8003802C(s32 arg0, Unk3802C *arg1, s32 arg2)
{
    s32 ret;
    u32 unk118;
    s32 delta;
    s32 limit;

    ret = func_80050D14(&arg1->unkDC, arg2, arg1);
    unk118 = arg1->unk118;
    if ((unk118 >> 30) == 1) {
        arg1->unkA4 |= 0x100;
        arg1->unkB0 = (arg1->unkB0 + (arg2 << 1)) & 0xFFF;
        if ((u32)(arg1->unkB0 - 1001) < 0x817) {
            arg1->unkB0 = 1000;
        }
        arg1->unkAC += arg2 >> 1;
        arg1->unkB4 = (arg1->unkB4 + arg2 * arg1->unkAA) & 0xFFF;
        limit = func_80066F78(&arg1->unk50, 0, 0, 0) + (arg1->unk68 >> 1);
        delta = arg1->unkAC * arg2;
        if (arg1->unk54 - delta < limit) {
            delta = arg1->unk54 - limit;
            arg1->unk1C = -1;
            arg1->unk118 |= 0xC0000000;
        }
        if (ret == 0) {
            arg1->unk54 -= delta;
        }
        ret = 2;
        arg1->unk102 -= delta >> 4;
    }

    if (ret != 0) {
        if ((ret & 2) != 0 || (unk118 & 0x200) != 0) {
            func_80037EB4(arg1);
        }
        if (ret != 0) {
            func_80010750(&arg1->unk2C);
            func_80010234((arg1->unk70 << 4) + 8 + arg1->unk28->unk34, &arg1->unk5C, 1);
        }
    }
}
