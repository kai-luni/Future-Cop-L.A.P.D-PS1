// Categories: Gameplay > Player > Handler | Gameplay > Entity Script
#include "game/types.h"

struct UnkDC {
    u8 pad00[0x24];
    u32 flags;
};

struct Unk5908C {
    u8 pad00[8];
    s32 unk08;
    u8 pad0C[0x10];
    u16 unk1C;
    u8 pad1E[0x8E];
    void *unkAC;
    u8 padB0[0x14];
    void *unkC4;
    u8 padC8[0x14];
    struct UnkDC *unkDC;
};

struct Unk5908CData {
    u8 pad00[8];
    struct Unk5908C *unk08;
};

extern s32 func_800579D4(struct Unk5908C *object, struct Unk5908CData *data, s32 arg3);
extern s32 func_800588D4(struct Unk5908C *object, struct Unk5908CData *data);
extern void func_80058D78(struct Unk5908C *object);
extern void func_800181D4(void *arg0);
extern s32 func_8002AD58(s32 message, struct Unk5908C *object, struct Unk5908CData *data);
extern void func_80058378(struct Unk5908C *object, s32 amount, s32 source);
extern s32 func_800584BC(s32 message, struct Unk5908C *object, struct Unk5908CData *hit,
                         s32 arg3, s32 arg4);

/**
 * @brief Message handler of the player unit (class 1): spawn, form tick,
 *        retire, damage/heal and the identity queries.
 *
 * @param message Handler case number.
 * @param object Unit block; +0x1C a counter word, +0xAC an owned block,
 *        +0xC4/+0xDC two identity pointers.
 * @param data Case payload.
 * @param arg4 Forwarded to spawn/damage helpers.
 * @return Case 1: 0xF4. Case 10: identity flag. Cases 3/4/5/9: 0. Other
 *         cases: the sub-helper or base results.
 *
 * @par Confirmed
 * Case 2 -> func_800579D4 (spawn); case 6 -> func_800588D4 (the form-change
 * tick); case 4 -> func_80058D78 (retire); case 5 frees unkAC; case 10
 * returns 0 when data matches unkC4 or unkDC, else 1. Case 9 ignores
 * self-hits (data->unk08 == object), else runs the base damage path and on
 * a positive result raises flag 0x02000000 on unkDC, adds the result to
 * unk1C and calls func_80058378 with the attacker chain. Cases 11/14 ->
 * func_800584BC; the rest fall through to func_8002AD58.
 *
 * @par Likely role
 * The player avatar's handler: case 9's positive-result branch is a
 * heal/score path (the counter grows), the identity pointers track the
 * rider and the controlled form.
 *
 * @par Unknown
 * What the unk1C counter measures (score or health).
 *
 * @par Callers
 * func_80053188, func_800578B4.
 */
s32 func_8005908C(s32 message, struct Unk5908C *object, struct Unk5908CData *data, s32 arg4)
{
    struct Unk5908C *s0;
    struct Unk5908CData *s1;
    s32 result;
    s32 flag;

    s0 = object;
    s1 = data;

    switch (message) {
    case 1:
        return 0xF4;
    case 2:
        func_800579D4(s0, s1, arg4);
        return 0;
    case 3:
        return 0;
    case 10:
        flag = 0;
        if (s0->unkC4 != s1) {
            flag = (s0->unkDC != s1);
        }
        return flag;
    case 6:
        return func_800588D4(s0, s1);
    case 4:
        func_80058D78(s0);
        return 0;
    case 5:
        if (s0->unkAC != 0) {
            func_800181D4(s0->unkAC);
            s0->unkAC = 0;
        }
        return 0;
    case 9:
        if (s1->unk08 == s0) {
            return 0;
        }
        result = func_8002AD58(message, s0, s1);
        if (result <= 0) {
            return 0;
        }
        s0->unkDC->flags |= 0x02000000;
        s0->unk1C += result;
        func_80058378(s0, result, s1->unk08 != 0 ? s1->unk08->unk08 : 0);
        return 0;
    case 11:
    case 14:
        return func_800584BC(message, s0, s1, arg4, 0);
    case 7:
    case 8:
    case 12:
    case 13:
    default:
        return func_8002AD58(message, s0, s1);
    }
}
