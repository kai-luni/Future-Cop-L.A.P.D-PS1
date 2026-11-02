#include "game/types.h"

extern s32 D_8009F7C0[];
extern u32 D_8009F7C4;
extern s32 D_8009FA0C;

void func_800576A8(u32 *out, s32 index)
{
    s32 in;
    u32 res;

    in = D_8009F7C0[index];

    res = -(u32)((in & 0x80000) != 0) & 0x80000;
    if (in & 0x40000) {
        res |= 0x40000;
    }
    if (in & 0x40000000) {
        res |= 0x40000000;
    }
    if (in & 0x80000000) {
        res |= 0x80000000;
    }
    if (in & 0x10000000) {
        res |= 0x10000000;
    }
    if (in & 0x20000000) {
        res |= 0x20000000;
    }
    if (in & 0x08000000) {
        res |= 0x04000000;
    }
    if (in & 0x04000000) {
        res |= 0x00400000;
    }
    if (in & 0x02000000) {
        res |= 0x01000000;
    }
    if (in & 0x01000000) {
        res |= 0x08000000;
    }
    if (in & 0x00400000) {
        res |= 0x02000000;
    }
    if (in & 0x00800000) {
        res |= 0x00800000;
    }
    if (in & 0x00100000) {
        res |= 0x00100000;
    }
    if (in & 0x00200000) {
        res |= 0x00200000;
    }

    if (D_8009FA0C == 1) {
        in = D_8009F7C4;
        if (in & 0x08000000) {
            res |= 0x04000000;
        }
        if (in & 0x04000000) {
            res |= 0x00400000;
        }
        if (in & 0x02000000) {
            res |= 0x01000000;
        }
    }

    *out = res;
}
