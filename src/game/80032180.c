#include "game/types.h"

struct Unk320E4 {
    u8 unk00[0x16];
    u32 kind;
    u8 unk17[0xB9];
    s32 baseX;
    s32 baseY;
    s32 baseZ;
    s32 targetX;
    s32 targetY;
    s32 targetZ;
    u8 unkE8[0x12];
    u8 state;
    u8 unkFB[0x11];
    s16 spreadZ;
    s16 spreadX;
};

struct AnimClip {
    u8 unk00[4];
    s16 start;
    s16 end;
    u8 unk08[8];
};

struct AnimTrack {
    struct AnimClip *clips;
    struct AnimClip *clip;
    s32 elapsed;
    s16 frame;
    s16 target;
    s8 step;
    u8 index;
    u8 count;
    u8 flags;
};

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk32180Sub {
    u8 pad00[0x10];
    u8 *unk10;
};

struct Unk32180Source {
    u8 pad00[0x34];
    u8 unk34;
    u8 unk35;
    u16 unk36;
    u8 unk38;
    u8 unk39;
    u16 unk3A;
    u16 unk3C;
    u16 unk3E;
    u16 unk40;
    u16 unk42;
    u16 unk44;
    s16 unk46;
    u16 unk48;
    u16 unk4A;
    s16 unk4C;
    s16 unk4E;
    s16 unk50;
    s16 unk52;
};

struct Unk32180Arg {
    u8 pad00[8];
    struct Unk32180Source *unk8;
};

struct Unk32180 {
    u8 pad00[0x28];
    struct Unk32180Sub *unk28;
    u8 unk2C[0x24];
    struct Vec3 pos;
    u8 pad5C[0x10];
    s16 unk6C;
    u8 pad6E[0x0A];
    s32 unk78;
    s32 unk7C;
    s32 unk80;
    s32 unk84;
    u8 pad88[0x40];
    s32 unkC8;
    u8 padCC[4];
    struct Vec3 unkD0;
    struct Vec3 unkDC;
    s32 unkE8;
    s32 unkEC;
    s32 unkF0;
    s32 unkF4;
    s16 unkF8;
    u8 unkFA;
    u8 unkFB;
    u16 unkFC;
    u16 unkFE;
    u16 unk100;
    s16 unk102;
    u8 unk104;
    u8 unk105;
    u16 unk106;
    u16 unk108;
    u16 unk10A;
    s16 unk10C;
    s16 unk10E;
    s16 unk110;
    s16 unk112;
    s16 unk114;
    s16 unk116;
    s32 unk118;
    s32 unk11C;
    struct AnimTrack unk120;
};

extern void func_8002AD58(s32, struct Unk32180 *, struct Unk32180Arg *);
extern void func_80060668(void *, void *);
extern void func_800320E4(struct Unk320E4 *);
extern s32 func_80066F78(s32 *, s32, s32, s32);
extern s32 func_80019668(void);
extern void func_8001846C(u32, s32 *, s32 *);
extern void func_800193F8(s32 *, void *);
extern void func_80044988(u8 *, struct AnimTrack *, u8, s32);

s32 func_80032180(s32 arg0, struct Unk32180 *obj, struct Unk32180Arg *arg)
{
    struct Unk32180Source *src;
    s32 *pos;
    s32 value;
    s32 sc[2];
    u32 kind;

    func_8002AD58(arg0, obj, arg);
    src = arg->unk8;
    func_80060668((u8 *)src + 0x24, (u8 *)obj + 0xA0);

    kind = src->unk38;
    obj->unkFA = kind;
    if (kind == 2) {
        obj->unkFA = 1;
    }

    obj->unkFB = src->unk39;
    obj->unkFC = (src->unk4C * 75) >> 6;
    obj->unkFE = (src->unk4E * 75) >> 6;
    obj->unk100 = obj->unkFA ? obj->unkFE : obj->unkFC;
    obj->unk102 = -1;
    obj->unk116 = -1;
    obj->unkC8 = 0;

    obj->unk104 = src->unk34;
    obj->unk105 = src->unk35;
    obj->unk106 = src->unk36;
    obj->unk10A = src->unk3C;
    obj->unk108 = src->unk3A;
    obj->unk10C = (s16)src->unk48 >> 5;
    obj->unk10E = (s16)src->unk4A >> 5;
    obj->unk110 = src->unk3E;
    obj->unk114 = src->unk40;
    obj->unk112 = src->unk42;
    obj->unkF8 = src->unk44;
    obj->unkF4 = (src->unk46 << 12) / 300;
    obj->unk84 = obj->unkF4 >> 8;

    if (obj->unkFB & 8) {
        obj->pos.z = src->unk50 << 9;
        obj->pos.x = src->unk52 << 9;
    }

    pos = &obj->pos.x;
    value = func_80066F78(pos, 0, 0, 0);
    obj->pos.y = value + (obj->unk114 << 4);
    obj->unkD0 = obj->pos;

    if (obj->unkFB & 1) {
        func_800320E4((struct Unk320E4 *)obj);
        obj->pos = obj->unkDC;
        value = func_80066F78(pos, 0, 0, 0);
        obj->pos.y = value + (obj->unk114 << 4);
    }

    if (obj->unkFB & 2) {
        obj->pos.y += 0x20000;
    } else if (obj->unkFB & 4) {
        obj->unkFA = 4;
        obj->pos.y = value + (obj->unk6C << 4);
    }

    obj->unkE8 = 0;
    obj->unkEC = 0;
    obj->unkF0 = 0;

    func_8001846C(func_80019668() & 0xFFF, &sc[0], &sc[1]);
    obj->unk78 = sc[0];
    obj->unk80 = sc[1];
    obj->unk7C = 0;
    func_800193F8(&obj->unk78, obj->unk2C);
    func_800320E4((struct Unk320E4 *)obj);

    obj->unk118 = (u32)0x80000000 / (u32)((obj->unk112 - obj->unk114) << 3);
    obj->unk11C = (u32)0x80000000 / (u32)((obj->unk114 - obj->unk110) << 3);

    if (obj->unk28->unk10 != 0) {
        func_80044988(obj->unk28->unk10, &obj->unk120, 1, 0);
    }

    return 0;
}
