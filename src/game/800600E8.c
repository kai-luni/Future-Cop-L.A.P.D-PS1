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

extern struct Unk600E8Found *func_8004E80C(struct Unk600E8Ctx *ctx,
    u8 arg1, s16 arg2, u8 arg3, s32 arg4, s32 arg5, s32 arg6);
extern void func_80069D68(struct Unk600E8Sub *sub, s32 arg1, u32 arg2,
    struct Unk600E8Ctx *ctx);

s32 func_800600E8(struct Unk600E8Obj *obj, s32 amount, struct Unk600E8Ctx *ctx)
{
    struct Unk600E8Found *found;

    obj->unk16 -= amount;
    if ((s16)obj->unk16 < 0) {
        obj->unk16 = obj->unk14;
        found = func_8004E80C(ctx, obj->unk06, obj->unk08, obj->unk07,
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
        func_80069D68(ctx->unk9C, (u32)ctx->unk1C >> 31,
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
