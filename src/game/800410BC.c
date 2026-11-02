#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Object;
typedef void (*ObjectFunc)(s32, struct Object *, void *, s32);

struct Object {
    u8 pad00[4];
    ObjectFunc unk04;
    s32 unk08;
    u8 pad0C[0x18];
    s32 unk24;
    u8 pad28[0x28];
    s32 unk50;
    u8 pad54[4];
    s32 unk58;
    u8 pad5C[4];
    s32 unk60;
    u8 pad64[4];
    s32 unk68;
};

struct Arg3 {
    u8 pad00[0x14];
    u16 unk14;
};

struct CallArgs {
    s32 unk0;
    struct Vec3s32 *unk4;
    struct Arg3 *unk8;
    s16 unkC;
    s16 unkE;
};

extern s32 D_8009F324;
extern u8 D_8009F328;
extern u8 D_8009F329;
extern s32 D_8009F700;
extern s32 D_8009F704;
extern s16 D_8009F710;
extern s16 D_8009F718;
extern s16 D_8009F71C;

extern struct Object *D_800A8F98[];

extern s32 func_8001A750(s32 *a, s32 *b);

#define MULU16(out, a, b) \
    __asm__ volatile("multu\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")
#define MUL16(out, a, b) \
    __asm__ volatile("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")

void func_800410BC(struct Vec3s32 *pos, s32 arg1, s32 arg2, struct Arg3 *arg3)
{
    struct CallArgs args;
    struct Object **list;
    struct Object *obj;
    s32 n;
    register s32 a0 __asm__("$4");
    s32 half;
    s32 r;
    s32 d;
    s32 s0;

    a0 = arg1 << 16;

    if (a0 > 0) {
        if (arg2 > 0) {
            half = arg2 >> 1;
            goto positive;
        }
        half = -arg2;
        half >>= 1;
        goto negative;
    }
    if (arg2 > 0) {
        half = arg2 >> 1;
        a0 = -a0;
    negative:
        MULU16(r, a0, 0x80000000u / (u32)half);
        D_8009F700 = -r;
    } else {
        a0 = -a0;
        half = -arg2;
        half >>= 1;
    positive:
        MULU16(r, a0, 0x80000000u / (u32)half);
        D_8009F700 = r;
    }

    D_8009F704 = arg2;

    if (D_8009F324 > 0) {
        args.unkC = 1;
    } else {
        args.unkC = 0;
    }

    args.unk4 = pos;

    if (arg3 != 0) {
        args.unk0 = arg3->unk14;
        args.unk8 = arg3;
    } else {
        args.unk0 = 0;
        args.unk8 = 0;
    }

    list = D_800A8F98;
    n = ((s32 *)list)[-2];

    for (; n > 0; n--) {
        obj = *list++;

        if (obj->unk08 <= 0) {
            continue;
        }
        if (obj->unk24 & 0x200080) {
            continue;
        }

        s0 = 0;
        d = func_8001A750((s32 *)pos, &obj->unk50);

        if (d < D_8009F704) {
            if (d >= D_8009F324) {
                if (D_8009F329 & 1) {
                    s32 yc = obj->unk60;
                    s32 yr = obj->unk68;
                    s32 y = pos->y;
                    if (yc + yr + 0x1000 < y) {
                        continue;
                    }
                    if (!(D_8009F329 & 2)) {
                        if (y < yc - yr - 0x1000) {
                            continue;
                        }
                    }
                }

                if (D_8009F328 != 0) {
                    s32 denom = d >> 8;
                    s32 tx = ((obj->unk50 - pos->x) << 4) / denom;
                    s32 tz = ((obj->unk58 - pos->z) << 4) / denom;
                    s32 sum = ((D_8009F718 * tx) >> 12) + ((D_8009F71C * tz) >> 12);
                    if (sum < D_8009F710) {
                        continue;
                    }
                }

                {
                    s32 k;
                    s0 = D_8009F704 - d;
                    k = D_8009F700;
                    MUL16(s0, s0, k);
                }
                s0 >>= 16;
            }
        }

        if (s0 != 0) {
            args.unkE = s0;
            obj->unk04(9, obj, &args, 0);
        }
    }
}
