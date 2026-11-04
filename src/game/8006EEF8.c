#include "game/types.h"

typedef struct {
    u8 pad00[4];
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    u8 pad10[0x18];
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s16 unk34;
    s16 unk36;
    s16 unk38;
    s16 unk3A;
    s16 unk3C;
    u16 unk3E;
    s32 unk40;
    s16 unk44;
} Unk6EEF8;
typedef struct { s32 x, y, z; } V3;

extern s32 func_8001A314(s32 *vec);

void func_8006EEF8(Unk6EEF8 *node, s32 scale, s16 arg2, s32 arg3, s32 arg4, s32 *p1, s32 *p2)
{
    s32 vec[3];
    s16 sv[3];
    s32 t;

    vec[0] = p2[0] - p1[0];
    vec[1] = p2[1] - p1[1];
    vec[2] = p2[2] - p1[2];
    t = func_8001A314(vec);
    sv[0] = vec[0] >> 4;
    sv[1] = vec[1] >> 4;
    sv[2] = vec[2] >> 4;
    node->unk34 = (sv[0] * scale) >> 12;
    node->unk36 = (sv[1] * scale) >> 12;
    node->unk38 = (sv[2] * scale) >> 12;
    t = (t >> 4) - scale;
    if (t < 0) {
        node->unk3C = 0;
    } else {
        node->unk3C = (t * 300) / (arg3 >> 4);
    }
    *(V3 *)&node->unk04 = *(V3 *)p1;
    arg3 = arg3 / 300;
    node->unk28 = (sv[0] * arg3) >> 8;
    node->unk2C = (sv[1] * arg3) >> 8;
    node->unk30 = (sv[2] * arg3) >> 8;
    node->unk3E = 0x8000;
    node->unk3A = 0;
    node->unk40 = arg4;
    node->unk44 = arg2;
}
