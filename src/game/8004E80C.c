#include "game/types.h"

struct Unk4E80C {
    u8 pad00[8];
    s32 unk08;
    u8 pad0C[8];
    u16 unk14;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19;
    u8 pad1A[0xA];
    u32 unk24;
    u8 pad28[0x34];
    u8 unk5C[1];
};

struct Unk4E80CList {
    s32 count;
    s32 pad04;
    struct Unk4E80C *items[1];
};

extern struct Unk4E80CList D_800A8F90;

extern s32 func_8004E5F0(struct Unk4E80C *a, struct Unk4E80C *b, s32 c, s32 d);
extern s32 func_800684AC(void *a, void *b, s32 c, s32 d, s32 e);
extern s32 func_80019668(void);

struct Unk4E80C *func_8004E80C(struct Unk4E80C *ctx, s32 type, s32 val, s32 mode,
                               s32 arg4, s32 arg5, u8 flag)
{
    struct Unk4E80C *best;
    struct Unk4E80C *o;
    s32 ctxId;
    s32 bestVal;
    s32 prio;
    s32 cnt;
    s32 r;
    s32 i;

    if (mode == 6) {
        return ctx;
    }
    cnt = 0;
    best = 0;
    ctxId = ctx->unk08;
    bestVal = arg4;
    if (flag) {
        prio = -1;
    }
    for (i = 0; i < D_800A8F90.count; i++) {
        o = D_800A8F90.items[i];
        if (o->unk08 <= 0) continue;
        if (o->unk08 == ctxId) continue;
        if (o == ctx) continue;
        if (o->unk24 & 1) continue;
        if (ctx->unk16 == 1 && (o->unk24 & 0x20000)) continue;
        switch (type) {
        case 2:
            if (o->unk14 != val) continue;
            break;
        case 1:
            if (o->unk16 != val) continue;
            break;
        case 3:
            if (o->unk17 != val) continue;
            break;
        case 4:
            if (o->unk18 != val) continue;
            break;
        case 5:
            if (!((1 << o->unk18) & val)) continue;
            break;
        case 7:
            if (!(o->unk14 & 0x4000)) continue;
            break;
        default:
            return 0;
        }
        switch (mode) {
        case 0:
            return o;
        case 1:
        case 3:
            if (flag) {
                if ((o->unk19 & 0xF) < prio) continue;
            }
            r = func_8004E5F0(ctx, o, arg4, arg5);
            if (r < 0) continue;
            if (r >= bestVal) {
                if (!flag) continue;
                if (!(prio < (o->unk19 & 0xF))) continue;
            }
            if (mode != 1) {
                if (func_800684AC(ctx->unk5C, o->unk5C, 0, 0, 0) != 0) continue;
            }
            bestVal = r;
            best = o;
            if (flag) {
                prio = o->unk19 & 0xF;
            }
            break;
        case 2:
            cnt++;
            break;
        default:
            return 0;
        }
    }
    if (mode != 2) {
        return best;
    }
    if (cnt > 0) {
    cnt = (u32)func_80019668() % (u32)cnt;
    for (i = 0; i < D_800A8F90.count; i++) {
        o = D_800A8F90.items[i];
        if (o->unk08 <= 0) continue;
        if (o->unk08 == ctxId) continue;
        switch (type) {
        case 2:
            if (o->unk14 != val) continue;
            break;
        case 1:
            if (o->unk16 != val) continue;
            break;
        case 3:
            if (o->unk17 != val) continue;
            break;
        case 4:
            if (o->unk18 != val) continue;
            break;
        case 5:
            if (!((1 << o->unk18) & val)) continue;
            break;
        case 7:
            if (!(o->unk14 & 0x4000)) continue;
            break;
        default:
            break;
        }
        cnt--;
        if (cnt < 0) {
            return o;
        }
    }
    }
    return 0;
}
