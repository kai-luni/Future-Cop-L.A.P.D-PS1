#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Source {
    u8 pad00[0x34];
    u8 unk34;
    s8 unk35;
    s16 unk36;
    union {
        u16 u;
        s16 s;
    } unk38;
    s16 unk3A;
    s16 unk3C;
    s16 unk3E;
    s16 unk40;
    s16 unk42;
    s16 unk44;
    u8 unk46;
    u8 unk47;
    u8 unk48;
    u8 unk49;
    u8 unk4A;
    u8 unk4B;
    u8 unk4C;
};

struct Sub {
    u8 pad00[0x10];
    u8 *unk10;
};

struct Obj {
    u8 pad00[0x20];
    u8 unk20;
    u8 unk21;
    u8 pad22[2];
    u32 flags;
    struct Sub *unk28;
    u8 pad2C[0x24];
    struct Vec3 pos;
    u8 pad5C[0x1C];
    struct Vec3 off;
    s32 unk84;
    u8 pad88[0x40];
    u8 unkC8[0x14];
    u8 unkDC;
    u8 unkDD;
    u16 unkDE;
    u16 unkE0;
    u16 unkE2;
    u16 unkE4;
    u8 unkE6;
    u8 padE7;
    s32 unkE8;
    s32 unkEC;
    s32 unkF0;
    struct Vec3 tmp;
    struct Vec3 unk100;
    u8 unk10C;
    u8 unk10D;
    u8 unk10E;
    u8 unk10F;
    u8 unk110;
    u8 unk111;
    u8 unk112;
    u8 unk113;
};

struct Arg {
    u8 pad00[8];
    struct Source *unk8;
};

extern s32 func_80067684(struct Vec3 *pos, s32 sel);
extern void func_800188D0(s32 yaw, s32 pitch, s32 roll, void *matrix);
extern s32 func_8001A100(s32 *vector);
extern void func_80044988(u8 *data, void *track, u8 forward, s32 index);
extern void func_80060668(void *in, void *out);

s32 func_8002DFC4(struct Obj *obj, struct Arg *arg)
{
    struct Source *st;
    s32 sel;
    u16 h;
    u8 t;

    st = arg->unk8;
    sel = st->unk35 == 0 ? -1 : st->unk35 == 1 ? -2 : st->unk35 - 2;
    obj->pos.y = func_80067684(&obj->pos, sel) + (st->unk36 << 4);
    obj->unkE4 = st->unk38.u;
    func_800188D0(st->unk38.s, 0, 0, (u8 *)obj + 0x2C);

    obj->unkE8 = (st->unk40 << 4) / 300;
    if (st->unk40 > 0 && obj->unkE8 == 0) {
        obj->unkE8 = 1;
    }

    obj->unk84 = 0;
    obj->unkDD = st->unk34;
    h = (st->unk42 << 12) / 218 + 1;
    obj->unkE2 = h;
    obj->unkDE = h;
    obj->unkE0 = (st->unk44 << 12) / 218 + 1;
    obj->unkDD = st->unk34;
    obj->unkE6 = 0;

    obj->unk10C = st->unk46;
    obj->unk10D = st->unk47;
    obj->unk10E = st->unk48;
    obj->unk10F = st->unk49;
    obj->unk110 = st->unk4A;
    obj->unk111 = st->unk4B | 1;
    obj->unk112 = st->unk4C;

    obj->off.x = st->unk3C << 4;
    obj->off.y = st->unk3E << 4;
    obj->off.z = st->unk3A << 4;

    if (obj->unkDD & 8) {
        obj->tmp.x = obj->pos.x + obj->off.x;
        obj->tmp.y = obj->pos.y + obj->off.y;
        obj->tmp.z = obj->pos.z + obj->off.z;
        obj->pos = obj->tmp;
        obj->off.x = -obj->off.x;
        obj->off.y = -obj->off.y;
        obj->off.z = -obj->off.z;
    } else {
        obj->tmp = obj->pos;
    }

    obj->unk100 = obj->off;
    obj->unkEC = 0;
    obj->unkF0 = func_8001A100(&obj->off.x);

    obj->unkDC = 0;
    if (obj->unkDD & 2) {
        obj->flags |= 0x80;
    }

    if (obj->unk28->unk10 != 0) {
        func_80044988(obj->unk28->unk10, obj->unkC8, 1, 0);
    }
    func_80060668((u8 *)st + 0x24, (u8 *)obj + 0xA0);

    if (obj->unkDD & 0x40) {
        t = obj->unk21;
        obj->unk21 = 0;
        obj->unk113 = t;
    }
    return 0;
}
