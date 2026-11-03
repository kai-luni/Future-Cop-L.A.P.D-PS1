#include "game/types.h"

struct Model;

struct Matrix3x3 {
    s32 m[3][3];
};

struct ModelInstance {
    struct Model *model;
    struct Matrix3x3 matrix;
};

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Basis {
    struct Vec3s32 rows[3];
};

struct State37064 {
    u8 unk00[0x08];
    struct Vec3s32 unk08;
    u8 unk14[0x04];
    s16 unk18;
    s16 unk1A;
    u8 unk1C[0x05];
    u8 unk21;
    u8 unk22[0x02];
    struct Model *unk24;
    struct Vec3s32 unk28;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    struct State37064 *unk44;
    u8 unk48;
    u8 unk49;
    s16 unk4A;
};

struct Holder37064 {
    u8 unk00[0x08];
    struct State37064 *state;
};

struct Dest37064 {
    u8 unk00[0x1A];
    s16 unk1A;
    s16 unk1C;
    u8 unk1E[0x02];
    u8 unk20;
    u8 unk21;
    u8 unk22[0x02];
    s32 unk24;
    struct ModelInstance unk28;
    struct Vec3s32 unk50;
    u8 unk5C[0x1C];
    struct Vec3s32 unk78;
    u8 unk84[0x1C];
    struct Vec3s32 unkA0;
    u8 unkAC[0x0C];
    s32 unkB8;
    s32 unkBC;
    s32 unkC0;
    s32 unkC4;
    struct State37064 *unkC8;
    s32 unkCC;
    u8 unkD0;
    u8 unkD1;
    s16 unkD2;
};

extern s32 func_8002AD58(s32 message, void *object, void *data, s32 arg4);
extern void func_80013BD8(struct ModelInstance *instance, struct Model *model, s32 options);
extern void func_8001A180(struct Vec3s32 *vector);
extern void func_800193F8(struct Vec3s32 *axis, struct Basis *basis);

void func_80037064(struct Dest37064 *dest, struct Holder37064 *holder, s32 arg2)
{
    struct State37064 *p;
    s32 w;

    func_8002AD58(2, dest, holder, arg2);
    p = holder->state;

    dest->unk20 = p->unk48;
    dest->unk50 = p->unk08;
    func_80013BD8(&dest->unk28, p->unk24, 0);
    dest->unk1C = p->unk18;
    dest->unk1A = p->unk1A;
    dest->unkB8 = p->unk34;
    dest->unkBC = p->unk38;
    dest->unkC0 = p->unk3C;
    dest->unkC4 = p->unk40;
    dest->unkC8 = p->unk44;
    dest->unk21 = p->unk21;
    dest->unkD2 = p->unk4A;

    if (dest->unkC8 != 0) {
        w = dest->unkC8->unk08.x;
        dest->unkCC = w;
        if (w <= 0) {
            dest->unkC8 = 0;
        }
    }

    dest->unkD0 = 0;
    dest->unk24 = (dest->unk24 | 0x840002) & 0xFFFFDFFF;

    dest->unkA0 = p->unk28;
    dest->unk78 = dest->unkA0;

    if (dest->unk78.x == 0 && dest->unk78.y == 0 && dest->unk78.z == 0) {
        dest->unk78.z = 0x10000;
    } else {
        func_8001A180(&dest->unk78);
    }

    func_800193F8(&dest->unk78, (struct Basis *)&dest->unk28.matrix);
}
