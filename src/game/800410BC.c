// Categories: Gameplay > Damage > Expanding Contact Field | Physics > Collision > Radius Tests | Gameplay > Object Dynamics
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
extern u8 gConeTestFlag __asm__("D_8009F328");
extern u8 gBandTestFlag __asm__("D_8009F329");
extern s32 gLightStrengthScale __asm__("D_8009F700");
extern s32 D_8009F704;
extern s16 gShadowThreshold __asm__("D_8009F710");
extern s16 D_8009F718;
extern s16 D_8009F71C;

extern struct Object *gEffectListHead[] __asm__("D_800A8F98");

extern s32 func_8001A750(s32 *a, s32 *b);

#define MULU16(out, a, b) \
    __asm__ volatile("multu\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")
#define MUL16(out, a, b) \
    __asm__ volatile("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")

/**
 * @brief Radial blast: walks the global object list, keeps the objects inside a
 *        radius, an optional vertical band and an optional cone, and sends each
 *        of them message 9 with a strength that falls off with distance.
 *
 * @param pos Blast centre, three s32 world coordinates.
 * @param arg1 Strength; only the low 16 bits survive the left shift that starts
 *        the function, so it is a 16-bit quantity.
 * @param arg2 Radius; objects at or beyond it are skipped and the remainder
 *        sets the falloff.
 * @param arg3 Optional source record: its +0x14 word becomes the message's first
 *        word and the record itself is the message payload. May be null.
 * @return Nothing.
 *
 * @par Confirmed
 * The four sign combinations of (arg1 << 16) and arg2 produce
 * r = |arg1 << 16| * (0x80000000 / (|arg2| >> 1)) and store +r or -r in
 * D_8009F700, with D_8009F704 = arg2. Both globals are only read by this
 * function: D_8009F704 is the radius and D_8009F700 the factor that turns a
 * remaining distance into a strength.
 * The message record is {source +0x14 or 0, pos, arg3 or 0, (D_8009F324 > 0),
 * strength}.
 * The list head is D_800A8F98 and the count is the longword two positions
 * before it, i.e. the count word of the table that precedes the array. Objects
 * with +0x08 <= 0 and objects with
 * +0x24 & 0x200080 are skipped. d = func_8001A750(pos, obj +0x50) is the planar
 * octagonal distance from the centre.
 * An object is only considered when d < radius and d >= D_8009F324: a positive
 * D_8009F324 acts as an inner dead zone, and the two wrappers func_800413EC and
 * func_80041428 are what set it, together with the option bits below.
 * Band test, only with bit 0 of D_8009F329: with yc = obj +0x60, yr = obj +0x68
 * and y = pos->y, an object is rejected when yc + yr + 0x1000 < y and, unless
 * bit 1 of D_8009F329 is set, also when y < yc - yr - 0x1000.
 * Cone test, only when D_8009F328 is non-zero: denom = d >> 8, tx = ((obj +0x50
 * - pos->x) << 4) / denom, tz = ((obj +0x58 - pos->z) << 4) / denom, and the
 * object is rejected when ((D_8009F718 * tx) >> 12) + ((D_8009F71C * tz) >> 12)
 * is below D_8009F710. func_80041428 fills those three half-words from a
 * direction shifted right by 4 and a height threshold.
 * Strength: s0 = radius - d, then s0 = (s0 * D_8009F700) >> 16 with the 32x32
 * middle-bits macro, so it is linear in the remaining distance. Only a non-zero
 * s0 sends anything: obj->+0x04(9, obj, &record, 0).
 *
 * @par Likely role
 * The shared explosion/contact-damage broadcaster. Callers compute an amount
 * from their own attack parameters (func_80036624 scales it by the distance
 * ratio), func_80036AB4 fires it as its "explode now" path, func_8006B23C runs
 * it per effect part, and func_8003E984 uses it once a spawned object asks for
 * it. Message 9 is the same damage channel the object handler family answers,
 * with the position and the strength in the payload. The optional band makes it a
 * blast that reaches only a vertical slice, and the optional cone a directional
 * one, which fits shockwaves, flamethrowers and sonic attacks sharing one
 * routine.
 *
 * @par Unknown
 * Whether D_8009F324 - named "context" by both wrappers - is really an inner
 * radius, what bit 1 of D_8009F329 means physically (dropping only the lower
 * half of the band test), what the +0x60/+0x68 pair holds beside the position at
 * +0x50, and what the receiver does with the flag word at +0x0C of the record.
 *
 * @par Callers
 * func_80036624, func_80036AB4, func_80037708, func_8003E984, func_800413EC,
 * func_80041428, func_8006B23C (two sites).
 */
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
        gLightStrengthScale = -r;
    } else {
        a0 = -a0;
        half = -arg2;
        half >>= 1;
    positive:
        MULU16(r, a0, 0x80000000u / (u32)half);
        gLightStrengthScale = r;
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

    list = gEffectListHead;
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
                if (gBandTestFlag & 1) {
                    s32 yc = obj->unk60;
                    s32 yr = obj->unk68;
                    s32 y = pos->y;
                    if (yc + yr + 0x1000 < y) {
                        continue;
                    }
                    if (!(gBandTestFlag & 2)) {
                        if (y < yc - yr - 0x1000) {
                            continue;
                        }
                    }
                }

                if (gConeTestFlag != 0) {
                    s32 denom = d >> 8;
                    s32 tx = ((obj->unk50 - pos->x) << 4) / denom;
                    s32 tz = ((obj->unk58 - pos->z) << 4) / denom;
                    s32 sum = ((D_8009F718 * tx) >> 12) + ((D_8009F71C * tz) >> 12);
                    if (sum < gShadowThreshold) {
                        continue;
                    }
                }

                {
                    s32 k;
                    s0 = D_8009F704 - d;
                    k = gLightStrengthScale;
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
