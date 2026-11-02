#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Self80052708 {
    u8 pad00[0xAC];
    u8 *unkAC;
    u8 padB0[8];
    u8 unkB8;
};

extern s32 func_80052410(struct Self80052708 *self, struct Vec3 *outA, struct Vec3 *outB);

void func_80052708(struct Self80052708 *self, s32 arg1)
{
    struct Vec3 outA[4];
    struct Vec3 outB[8];
    s32 mask;
    s32 seen;
    s32 i;
    s32 bits;
    s32 flag;
    s32 avgX;
    s32 avgZ;
    s32 dx;
    s32 dz;
    s32 half;
    u8 *p;

    p = self->unkAC;
    mask = func_80052410(self, outA, outB);

    ((struct Vec3 *)(p + 0x10C))[0] = outA[0];
    ((struct Vec3 *)(p + 0x10C))[1] = outA[1];
    ((struct Vec3 *)(p + 0x10C))[2] = outA[2];
    ((struct Vec3 *)(p + 0x10C))[3] = outA[3];

    if (mask == 0) {
        return;
    }

    p += 0xEC;

    avgX = (outA[0].x + outA[1].x + outA[2].x + outA[3].x) >> 2;
    avgZ = (outA[0].z + outA[1].z + outA[2].z + outA[3].z) >> 2;

    seen = 0;
    for (i = 7; i >= 0; i--) {
        bits = mask & 7;
        mask >>= 3;
        if (bits == 0) {
            continue;
        }
        flag = 1 << bits;
        if (seen & flag) {
            continue;
        }
        seen |= flag;
        self->unkB8 = bits;

        dx = avgX - outB[i].x;
        dz = avgZ - outB[i].z;

        switch (bits) {
        case 1:
            dz = 0;
            break;
        case 2:
            dx = 0;
            break;
        case 3:
            half = (dx + dz) / 2;
            dz = half / 2;
            dx = dz;
            break;
        case 4: {
            s32 d = dx - dz;
            half = d / 2;
            dx = half / 2;
            dz = -half / 2;
            break;
        }
        default:
            break;
        }

        if (dx < 0) {
            dx = -0x24000;
        } else if (dx > 0) {
            dx = 0x24000;
        }

        if (dz < 0) {
            dz = -0x24000;
        } else if (dz > 0) {
            dz = 0x24000;
        }

        if (arg1 != 0) {
            dx <<= 1;
            dz <<= 1;
        }

        {
            s32 vx = *(s32 *)(p + 0x64);
            s32 vz = *(s32 *)(p + 0x6C);
            *(s32 *)(p + 0x64) = vx + dx;
            *(s32 *)(p + 0x6C) = vz + dz;
        }
    }
}
