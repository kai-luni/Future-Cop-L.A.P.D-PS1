#include "game/types.h"

struct Mat3 {
    s32 m[3][3];
};

struct Sub {
    u8 pad00[4];
    struct Mat3 mat;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s16 unk44;
    s16 unk46;
};

struct Ent {
    u8 pad00[0x28];
    struct Sub sub;
    u8 pad70[0x42];
    s16 unkB2;
    s16 unkB4;
    s16 unkB6;
};

extern void func_80019470(struct Mat3 *source, struct Mat3 *destination);
extern void func_80010820(void *matrix);
extern void func_8001006C(s32 *source, s32 *destination, s32 count);

s32 func_80031238(struct Ent *self, struct Ent *other)
{
    register struct Ent *pa __asm__("$18");
    register struct Ent *pb __asm__("$7");
    register struct Sub *sa __asm__("$16");
    register struct Sub *sb __asm__("$17");
    struct Mat3 matrix;
    s32 r[3];
    s32 dx, dy;
    s32 gx, gz, gy;
    register s32 firstsign __asm__("$4");
    s32 tx, tz, ty;
    s32 v;
    register s32 cond2 __asm__("$3");
    s32 sum, sum3;
    s32 x;
    s32 q;
    s32 ab;
    register s32 first __asm__("$3");
    s32 radius;
    register s32 sign1 __asm__("$3");
    register s32 norm1 __asm__("$5");
    register s32 tmp1 __asm__("$4");

    pa = self;
    pb = other;
    sa = &pa->sub;
    sb = &pb->sub;

    dx = sa->unk34 - sb->unk34;
    firstsign = dx >> 31;
    tx = firstsign + dx;
    sum = (sa->unk46 + sb->unk46) * 16;
    dx = tx ^ firstsign;
    __asm__ volatile ("" : : : "$4");
    if (sum < dx) {
        return 0;
    }

    gx = sa->unk3C - sb->unk3C;
    __asm__ volatile ("" : : : "$2");
    gz = gx >> 31;
    tz = gz + gx;
    gx = tz ^ gz;
    if (sum < gx) {
        return 0;
    }

    dy = sa->unk38 - sb->unk38;
    gy = dy >> 31;
    ty = gy + dy;
    sum3 = (sa->unk44 + sb->unk44) * 16;
    dy = ty ^ gy;
    if (sum3 < dy) {
        return 0;
    }

    func_80019470(&pb->sub.mat, &matrix);

    r[0] = sb->unk34 - sa->unk28;
    r[1] = sb->unk38 - sa->unk2C;
    r[2] = sb->unk3C - sa->unk30;

    func_80010820(&matrix);
    func_8001006C(r, r, 1);

    first = r[0];
    {
        register s32 rad __asm__("$6");
        register s32 sum1 __asm__("$2");
        rad = sb->unk46;
        __asm__ volatile ("" : "+r" (rad));
        norm1 = first >> 4;
        sign1 = first >> 31;
        tmp1 = sign1 + norm1;
        norm1 = tmp1 ^ sign1;
        sum1 = rad + pa->unkB2;
        if (sum1 < norm1) {
            return 0;
        }
        {
            s32 raw2;
            register s32 norm2 __asm__("$4");
            register s32 limit2 __asm__("$3");
            raw2 = r[2];
            limit2 = pa->unkB6;
            norm2 = raw2 >> 4;
            raw2 = raw2 >> 31;
            norm2 = raw2 + norm2;
            norm2 = norm2 ^ raw2;
            __asm__ volatile ("" : "+r" (norm2), "+r" (limit2), "+r" (rad));
            limit2 = rad + limit2;
            __asm__ volatile ("" : "+r" (limit2));
            sum = limit2;
            ab = norm2;
        }
    }
    cond2 = sum < ab;
    __asm__ volatile ("" : "+r" (cond2));
    if (cond2) {
        return 0;
    }

    {
        register s32 last __asm__("$3");
        register s32 val __asm__("$5");
        register s32 lim __asm__("$2");
        register s32 extra __asm__("$4");
        last = r[1];
        lim = pa->unkB4;
        extra = sb->unk44;
        val = last >> 4;
        last = last >> 31;
        val = last + val;
        val = val ^ last;
        lim = lim + extra;
        return lim >= val;
    }
}
