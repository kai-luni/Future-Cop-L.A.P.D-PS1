// Categories: Gameplay > Weapons | Graphics > Effects > Dispatcher | Platform > Sound
#include "game/types.h"

extern int gWeaponSlotUpdate();

struct V3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Owner6ACB0 {
    u8 pad00[0xA0];
    s32 x;
    s32 y;
    s32 z;
    u8 *state;
    u8 padB0[0x2C];
    u8 *unkDC;
};

struct Tmp60 {
    u8 pad00[0x10];
    struct V3 pos;
    u8 pad1C[0x28];
    void *ptr;
    u8 a;
    u8 b;
    u8 pad4A[6];
};

struct Locals6ACB0 {
    struct V3 vec;
    u8 pad3C[4];
    struct V3 posA;
    u8 pad4C[4];
    struct V3 posB;
    u8 pad5C[4];
    struct Tmp60 t60;
};

extern s32 gCommandListHead[2] __asm__("D_800A8F90");

extern void gTransformPartVertex(void *instance, s32 part, void *out) __asm__("func_80017668");
extern void gNormalizeVector(void *arg0) __asm__("func_8001A180");
extern void gWriteCellEventValue(void *object, void *event) __asm__("func_8005C048");
extern s32 func_80046C84(s32 id, s32 arg1);
extern void *gRegistryFind(u8 id) __asm__("func_8006AAEC");
extern s32 gSpawnTypeAtOwner(void *owner, u8 column, void *arg2, s32 row, s32 arg4) __asm__("func_800402DC");

extern void gSolveLeadDirection(void *a0, void *a1, s32 a2, s32 a3, void *a4, void *a5, s32 a6) __asm__("func_8005E2A0");
extern s32 gSpawnDirectShot(void *a0, s32 a1, void *a2, void *a3, void *a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11) __asm__("func_8005E47C");
extern s32 func_8005EE04(void *a0, void *a1, void *a2, void *a3, void *a4, s32 a5, s32 a6);
extern s32 gSolveLobVector(s32 a0, void *a1, void *a2, void *a3, s32 a4) __asm__("func_8005E624");
extern s32 gSpawnLobShot(s32 a0, void *a1, void *a2, void *a3, void *a4, s32 a5, void *a6, s32 a7, s32 a8) __asm__("func_8005E9E4");
extern s32 func_8005ECB8(void *a0, void *a1, void *a2, void *a3, s32 a4);
extern s32 func_8005EB74(void *a0, void *a1, void *a2, void *a3, s32 a4, void *a5, s32 a6);
extern s32 gAttachPartEffect(void *a0, void *a1, s32 a2, void *a3, s32 a4, void *a5) __asm__("func_8005CBCC");
extern s32 gWeaponEffectUpdate(void *a0, void *a1, void *a2) __asm__("func_8005D690");

/**
 * @brief Fires one weapon slot of an owner: resolves the alternating muzzle,
 *        looks up the weapon config, plays its sound and dispatches the
 *        action-specific effect spawner.
 *
 * @param owner Owner block; +0xA0 the position triple, +0xAC the state
 *        block, +0xDC a model holder.
 * @param index Weapon slot index.
 * @return 1 when the fire sequence ran, 0 when the pool gate fails.
 *
 * @par Confirmed
 * Gate: D_800A8F90[1] - D_800A8F90[0] >= 10 (free pool entries). The muzzle
 * offset is the parity of the s16 at state + 0x8A + index * 2 when positive,
 * else of state + 0x84 + index * 2 - the two 76-byte model records at
 * state + 0x178 alternate left/right. posA = func_80017668(record, 0);
 * config = gRegistryFind(state[0x90 + index]); index 1 stamps 0x84 at
 * state + 0x174 + offset * 2. posB is the target model's (+0x6C) +0x5C
 * position, else posA + owner position. def = config + 0x24 when the s16
 * is positive, else config + 0x04. The sound def[0x1C] plays via
 * func_80046C84; action 2 without a target model becomes 1. Actions
 * 9/1/3/0x12/0x13/0x15 normalise (posB - posA) via func_8001A180. The
 * switch: 1/2/3/0x12/0x13/0x15 -> gSpawnDirectShot (action 2 with the lead
 * helper gSolveLeadDirection first), 8 -> func_8005EE04, 9 -> gSolveLobVector +
 * func_8005EE04, 4/0x14 -> gSpawnLobShot, 5 -> func_8005ECB8, 6 ->
 * func_8005EB74, 0xF -> gWriteCellEventValue, 0x11 -> gAttachPartEffect, 0xD ->
 * gWeaponEffectUpdate. Flags s4 = (def[1] & 1) << 19 | 0x201. With def[1] & 4 the
 * result pointer is stored at state + 0x98/0x9C. The tail spawns the
 * secondary object through func_800402DC (column def[0x1B], row 1) either
 * on the result or on a local 0x60-byte record with the muzzle position.
 *
 * @par Likely role
 * The player/vehicle weapon fire path (caller gWeaponSlotUpdate): the same
 * spawner family as the actor attack dispatcher func_8005F6E8, driven by
 * the equipped weapon config instead of an AI descriptor.
 *
 * @par Unknown
 * The pool behind D_800A8F90 and the exact config layout beyond the used
 * offsets.
 *
 * @par Callers
 * gWeaponSlotUpdate.
 */
s32 gWeaponFireDispatch(struct Owner6ACB0 *owner, s32 index) __asm__("func_8006ACB0");

s32 gWeaponFireDispatch(struct Owner6ACB0 *owner, s32 index)
{
    struct Locals6ACB0 locals;
    register u8 *state __asm__("$19");
    u8 *config;
    u8 *def;
    u8 *result;
    s32 offset;
    s32 s4;
    s32 action;

    result = 0;
    if (gCommandListHead[1] - gCommandListHead[0] < 10) {
        return result;
    }

    state = owner->state;

    {
        s16 h;
        h = *(s16 *)(state + (index << 1) + 0x8A);
        if (h > 0) {
            offset = *(u16 *)(state + (index << 1) + 0x8A) & 1;
        } else {
            offset = *(u16 *)(state + (index << 1) + 0x84) & 1;
        }
    }

    gTransformPartVertex((void *)(state + 0x178 + offset * 76), 0, &locals.posA);

    config = (u8 *)gRegistryFind(*(u8 *)(index + state + 0x90));

    if (index == 1) {
        *(u16 *)(state + 0x174 + offset * 2) = 0x84;
    }

    {
        u8 *model;
        model = *(u8 **)(state + 0x6C);
        if (model != 0) {
            locals.posB = *(struct V3 *)(model + 0x5C);
        } else {
            locals.posB.x = locals.posA.x + owner->x;
            locals.posB.y = locals.posA.y + owner->y;
            locals.posB.z = locals.posA.z + owner->z;
        }
    }

    if (*(s16 *)(state + (index << 1) + 0x8A) > 0) {
        def = config + 0x24;
    } else {
        def = config + 0x04;
    }

    {
        u8 id2;
        id2 = *(u8 *)(def + 0x1C);
        action = *(u8 *)(def + 0x00);
        func_80046C84(id2, (s32)owner);
    }

    if (action == 2 && *(u8 **)(state + 0x6C) == 0) {
        action = 1;
    }

    if (action == 9 || action == 1 || action == 3 ||
        action == 0x12 || action == 0x13 || action == 0x15) {
        locals.vec.x = locals.posB.x - locals.posA.x;
        locals.vec.y = locals.posB.y - locals.posA.y;
        locals.vec.z = locals.posB.z - locals.posA.z;
        gNormalizeVector(&locals.vec);
    }

    s4 = (*(u8 *)(def + 1) & 1) << 19;
    s4 |= 0x201;

    switch (action) {
    case 1:
    case 2:
    case 3:
    case 0x12:
    case 0x13:
    case 0x15:
        {
            s32 t1;
            s32 t2;
            s32 t0;
            u8 flags;
            u8 *p;
            s32 a1v;
            u32 m2;
            u8 *ap;

            if (action == 2) {
                gSolveLeadDirection(&locals.posA, &locals.vec,
                              *(s16 *)(def + 0x12),
                              (s32)*(s16 *)(def + 0x16) << 4,
                              &locals.posB,
                              (void *)(*(u8 **)(state + 0x6C) + 0x78),
                              *(s32 *)(*(u8 **)(state + 0x6C) + 0x84));
                t1 = (s32)*(u8 **)(state + 0x6C);
                t2 = 0;
            } else {
                if (action == 3 || action == 0x12 || action == 0x13 || action == 0x15) {
                    t1 = (s32)*(u8 **)(state + 0x6C);
                } else {
                    t1 = 0;
                }
                t2 = *(u8 *)(def + 0x1B);
            }

            flags = *(u8 *)(def + 1);
            m2 = flags & 2;
            t0 = m2 != 0;
            if (flags & 8) {
                t0 |= 2;
            }

            ap = def + 0x0C;
            a1v = 0;
            p = *(u8 **)(config + 0x44);
            if (p != 0) {
                a1v = *(s32 *)p;
            }

            result = (u8 *)gSpawnDirectShot(ap, a1v, &locals.posA, &locals.vec,
                                         owner->unkDC, t1, s4, action,
                                         0xFF, 0, t2, t0);
        }
        break;

    case 8:
        result = (u8 *)func_8005EE04(def + 0x0C, config + 0x44, &locals.posA,
                                     owner->unkDC, &locals.vec, 0, s4);
        break;

    case 9:
        gSolveLobVector((s32)*(s16 *)(def + 0x12) << 4, &locals.vec, &locals.posA,
                      &locals.posB, (s32)*(s16 *)(def + 0x16) << 4);
        result = (u8 *)func_8005EE04(def + 0x0C, config + 0x44, &locals.posA,
                                     owner->unkDC, &locals.vec, 1, s4);
        break;

    case 4:
    case 0x14:
        result = (u8 *)gSpawnLobShot(action, def + 0x0C, config + 0x44, &locals.posA,
                                     owner->unkDC,
                                     (s32)*(u8 **)(state + 0x6C), &locals.posB, s4,
                                     *(u8 *)(def + 0x1B));
        break;

    case 5:
        result = (u8 *)func_8005ECB8(def + 0x0C, config + 0x44, &locals.posA,
                                     owner->unkDC, s4);
        break;

    case 6:
        result = (u8 *)func_8005EB74(def + 0x0C, config + 0x44, &locals.posA,
                                     owner->unkDC,
                                     (s32)*(u8 **)(state + 0x6C), &locals.posB, s4);
        break;

    case 0xF:
        gWriteCellEventValue(owner, def + 0x0C);
        return 1;

    case 0x11:
        gAttachPartEffect(owner, owner, (s32)*(u8 **)(owner->state + 0x6C),
                      state + 0x178 + offset * 76, 0, def + 0x0C);
        return 1;

    case 0xD:
        return gWeaponEffectUpdate(owner, def + 0x0C, def) & 0xFF;
    }

    if (result != 0 && (*(u8 *)(def + 1) & 4)) {
        *(s32 *)(state + 0x98) = (s32)result;
        *(s32 *)(state + 0x9C) = *(s32 *)(result + 8);
    }

    gTransformPartVertex((void *)(owner->unkDC + 0x28), offset, &locals.posA);

    if (*(u8 *)(def + 0x1B) != 0) {
        if (result != 0) {
            gSpawnTypeAtOwner(result, *(u8 *)(def + 0x1B), 0, 1, 0);
        } else {
            locals.t60.pos = locals.posA;
            locals.t60.ptr = (void *)(state + 0x178 + offset * 76);
            locals.t60.a = 0;
            locals.t60.b = 0x64;
            gSpawnTypeAtOwner(0, *(u8 *)(def + 0x1B), &locals.t60, 1, 0);
        }
    }

    __asm__("" : : "r"(state));
    return 1;
}
