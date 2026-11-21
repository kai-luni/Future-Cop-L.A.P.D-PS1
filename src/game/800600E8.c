// Categories: AI > Target Selection | Gameplay > Entity Script
#include "game/types.h"

struct Unk600E8Sub {
    u8 unk00;
};

struct Unk600E8Ctx {
    u8 unk00[0x1C];
    s16 unk1C;
    u8 unk1E[0x06];
    u32 unk24;
    u8 unk28[0x74];
    struct Unk600E8Sub *unk9C;
};

struct Unk600E8Found {
    u8 unk00[8];
    s32 unk08;
    u8 unk0C[8];
    u16 unk14;
};

struct Unk600E8Obj {
    u8 unk00[4];
    u16 flags;
    u8 unk06;
    u8 unk07;
    s16 unk08;
    s16 unk0A;
    s32 unk0C;
    u8 unk10[4];
    u16 unk14;
    u16 unk16;
    struct Unk600E8Found *unk18;
    s32 unk1C;
};

extern struct Unk600E8Found *gFindTargetEntity(struct Unk600E8Ctx *ctx,
    u8 arg1, s16 arg2, u8 arg3, s32 arg4, s32 arg5, s32 arg6) __asm__("func_8004E80C");
extern void gInterpretEntityScript(struct Unk600E8Sub *sub, s32 arg1, u32 arg2,
    struct Unk600E8Ctx *ctx) __asm__("func_80069D68");
s32 gAcquireAttackTarget(struct Unk600E8Obj *obj, s32 amount, struct Unk600E8Ctx *ctx) __asm__("func_800600E8");


/**
 * @brief Decrements a target-search timer, queries func_8004E80C when due, runs an acquisition script, and caches the target pointer and id.
 *
 * Confirmed: subtracts amount from obj->unk16 (+0x16); if signed result < 0, reloads +0x16 from obj->unk14 and calls func_8004E80C(ctx, unk06, unk08, unk07, unk0A << 4, unk0C, (flags >> 6) & 1). On null or non-expired timer, returns 0 (clearing obj->unk18 unless flags & 0x4000). On hit, skips script/write if (flags & 0x4000) && found == unk18 && found->unk08 == unk1C; otherwise runs func_80069D68 on ctx->unk9C if active and bit 0x100000 of ctx->unk24 is clear, then stores unk18 = found and unk1C = found->unk08. Category AI > Target Selection | Gameplay > Entity Script follows func_8004E80C search, cached target +0x18/+0x1C, and func_80069D68 script.
 * Likely: periodic target-acquisition helper for weapon/turret descriptors; unk9C is an on-acquire script. Unknown: exact meaning of flags bit 0x4000 vs bit 6 and ctx->unk24 bit 0x100000.
 *
 * @param obj Attack/target descriptor (+0x04 flags, +0x06..+0x0C filter params, +0x14 period, +0x16 timer, +0x18/+0x1C cached target).
 * @param amount Elapsed time/frame delta subtracted from obj->unk16.
 * @param ctx Owner entity (+0x1C s16 state, +0x24 flags, +0x9C script pointer) forwarded to func_8004E80C and func_80069D68.
 * @return 1 if a valid target was found on this search tick, 0 otherwise.
 * @par Globals
 * None.
 * @par Control flow
 * Timer countdown gate (< 0 triggers search); cleanup path clears unk18 when bit 0x4000 is clear; hit path short-circuits on unchanged target under 0x4000.
 * @par Calls
 * func_8004E80C - searches D_800A8F90 candidates by class, range, and mode. func_80069D68 - executes ctx->unk9C script on target acquisition.
 * @par Side effects
 * Updates obj->unk16, obj->unk18, obj->unk1C, and runs ctx->unk9C script side effects when triggered.
 * @par Callers
 * Confirmed: func_80060238 (0x800605AC, weapon/attack tick fire gate) / Likely: none / Unknown: none.
 */
s32 gAcquireAttackTarget(struct Unk600E8Obj *obj, s32 amount, struct Unk600E8Ctx *ctx)
{
    struct Unk600E8Found *found;

    obj->unk16 -= amount;
    if ((s16)obj->unk16 < 0) {
        obj->unk16 = obj->unk14;
        found = gFindTargetEntity(ctx, obj->unk06, obj->unk08, obj->unk07,
                              obj->unk0A << 4, obj->unk0C,
                              (obj->flags >> 6) & 1);
        if (found != 0) {
            goto found_nonnull;
        }
    }

cleanup:
    if (obj->flags & 0x4000) {
        return 0;
    }
    obj->unk18 = 0;
    return 0;

found_nonnull:
    if ((obj->flags & 0x4000) && found == obj->unk18 &&
        found->unk08 == obj->unk1C) {
        return 1;
    }
    if (ctx->unk9C != 0 && ctx->unk9C->unk00 != 0 &&
        !(ctx->unk24 & 0x100000)) {
        gInterpretEntityScript(ctx->unk9C, (u32)ctx->unk1C >> 31,
                      found->unk14 & 0x3FFF, ctx);
    }
    obj->unk18 = found;
    {
        register s32 tmp __asm__("$2");
        tmp = found->unk08;
        obj->unk1C = tmp;
    }
    return 1;
}
