#include "game/types.h"

typedef struct {
    u8 unk00[0x8C];
    u8 flags;
    u8 unk8D;
    u8 unk8E;
    u8 a8F;
    u8 a90;
    u8 a91;
    u8 a92;
    u8 a93;
    u8 a94;
    u8 unk95;
    s16 v96;
    s16 v98;
    s16 v9A;
    u8 unk9C[0x0A];
    s16 a6;
} Unk43250;

void func_80043250(Unk43250 *p)
{
    if (p->flags & 0x01) {
        if (p->a6 < 0x10) {
            p->v96 = 0;
            p->v98 = 0;
            p->v9A = 0;
        } else {
            s32 v = ((p->a8F + p->a90 + p->a91) << 12) / p->a6;
            p->v96 = v;
            p->v98 = v;
            p->v9A = v;
        }
    } else {
        if (p->a6 < 0x10) {
            p->v96 = 0;
            p->v98 = 0;
            p->v9A = 0;
        } else {
            p->v96 = ((p->a92 - p->a8F) << 12) / p->a6;
            p->v98 = ((p->a93 - p->a90) << 12) / p->a6;
            p->v9A = ((p->a94 - p->a91) << 12) / p->a6;
        }
    }

    if (p->v96 < 0) {
        u8 tmp = p->a92;
        p->a92 = p->a8F;
        p->a8F = tmp;
    }
    if (p->v98 < 0) {
        u8 tmp = p->a93;
        p->a93 = p->a90;
        p->a90 = tmp;
    }
    if (p->v9A < 0) {
        u8 tmp = p->a94;
        p->a94 = p->a91;
        p->a91 = tmp;
    }
}
