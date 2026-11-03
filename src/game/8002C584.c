#include "game/types.h"

struct Unk2C584Def {
    void *handler;
    void *buf;
    void *state;
    s32 zero;
};

struct Unk2C584Ent {
    u8 unk00[0x24];
    u32 flags;
    u8 unk28[0xC];
    s32 unk34;
};

struct V2 {
    s32 w[2];
};

struct V4 {
    struct V2 a;
    struct V2 b;
};

struct Unk2C584Inner {
    u8 unk00[4];
    struct V4 unk04;
    struct V4 unk14;
    struct V4 unk24;
};

struct Unk2C584Buf {
    s32 unk00;
    struct V4 unk04;
};

struct Unk2C584Data {
    u8 unk00[4];
    struct Unk2C584Inner *unk04;
    s32 *unk08;
};

struct Unk2C584State {
    u8 unk00[4];
    u8 unk04;
    u8 unk05[0x1B];
    u8 unk20;
    u8 unk21;
    u8 unk22[2];
    u8 unk24[0x10];
    u8 unk34;
    u8 unk35;
    u16 unk36;
    s16 unk38;
    s16 unk3A;
    s16 unk3C;
    u16 unk3E;
    u8 unk40;
    u8 unk41;
    u16 unk42;
    u8 unk44[4];
    u8 unk48[6];
    u16 unk4E;
};

struct Unk2C584Obj {
    u8 unk00[4];
    u8 unk04[0x20];
    u32 flags;
    struct Unk2C584Ent *unk28;
    u8 unk2C[0x24];
    u8 unk50[4];
    s32 unk54;
    u8 unk58[4];
    u8 unk5C[0x14];
    s16 unk70;
    u8 unk72[0x2E];
    u8 unkA0[0x28];
    u8 unkC8[0x14];
    u8 unkDC_pad[0];
    u16 unkDC;
    u8 unkDE;
    u8 unkDF;
    u32 unkE0;
    u8 unkE4[4];
    s16 unkE8;
    u16 unkEA;
    u16 unkEC;
    u8 unkEE[2];
    u16 unkF0;
    u16 unkF2;
    u16 unkF4;
    u8 unkF6;
    u8 unkF7;
    u8 unkF8;
    u8 unkF9;
    u8 unkFA;
    u8 unkFB;
    u16 unkFC;
    u8 unkFE;
    u8 unkFF;
    struct Unk2C584Obj *unk100;
    struct Unk2C584Obj *unk104;
    s32 unk108[4];
};

extern u8 D_8002DEA8[];

extern s32 func_8002AD58(s32 message, struct Unk2C584Obj *obj, void *data, s32 arg3);
extern struct Unk2C584Obj *func_8004D444(void *def);
extern void func_80017668(void *a, s32 b, void *c);
extern void func_80010750(void *a);
extern void func_80010234(void *a, void *b, s32 c);
extern void func_800188D0(s32 a, s32 b, s32 c, void *d);
extern void func_80060668(void *a, void *b);
extern s32 func_80067684(void *a, s32 b);
extern void func_80044988(void *a, void *b, s32 c, s32 d);
extern s32 func_8006314C(void *a, s32 b);

void func_8002C584(struct Unk2C584Obj *obj, struct Unk2C584Data *data, s32 arg2)
{
    struct Unk2C584State *st;
    struct Unk2C584Data *d;
    struct Unk2C584Def def;
    struct Unk2C584Buf buf;
    u32 s7;
    u32 s6;
    u32 s3;
    s32 s0;
    struct Unk2C584Inner *p;
    struct Unk2C584Inner *p2;
    s32 *q;
    struct V4 tmp;
    s32 i;
    s32 off;
    s32 *pp;
    s32 **r;

    d = data;
    st = (struct Unk2C584State *)data->unk08;
    s7 = st->unk20;
    if (st->unk40 & 0x80) {
        st->unk20 = 0;
    }
    func_8002AD58(2, obj, data, arg2);
    if (st->unk04 == 8 || st->unk04 == 0x21 || st->unk04 == 0x24) {
        p = d->unk04;
        def.handler = D_8002DEA8;
        def.buf = &buf;
        def.zero = 0;
        def.state = st;
        s6 = st->unk21;
        {
            u32 t = 0;
            if (!(st->unk40 & 0x40)) {
                t = s7;
            }
            st->unk20 = t;
        }
        st->unk21 = 0;
        buf.unk04.a = p->unk14.a;
        buf.unk04.b = p->unk14.b;
        obj->unk104 = func_8004D444(&def);
        *(struct Unk2C584Obj **)((u8 *)obj->unk104 + 0xB4) = obj;
        obj->unk104->flags |= 0x200000;
        obj->unk104->flags |= 0x800000;
        obj->flags |= 0x800000;
        s3 = st->unk04;
        if (s3 == 0x21) {
            s0 = st->unk38;
            st->unk38 = st->unk42;
            p2 = d->unk04;
            tmp.a = p2->unk04.a;
            tmp.b = p2->unk04.b;
            p2->unk04.a = p2->unk24.a;
            p2->unk04.b = p2->unk24.b;
            st->unk04 = 6;
            obj->unk100 = func_8004D444(d);
            st->unk38 = s0;
            st->unk04 = s3;
            obj->unk100->unk100 = obj;
            obj->unk100->unk104 = obj->unk104;
            obj->unk100->flags |= 0x800000;
            func_80017668((u8 *)obj->unk104 + 0x28, 1, (u8 *)obj->unk100 + 0x50);
            func_80010750((u8 *)obj->unk100 + 0x2C);
            func_80010234((u8 *)obj->unk100->unk28->unk34 + ((obj->unk100->unk70 << 4) + 8),
                          (u8 *)obj->unk100 + 0x5C, 1);
        } else {
            obj->unk100 = 0;
        }
        st->unk21 = s6;
        if (st->unk04 == 0x24) {
            obj->unkF6 = st->unk48[0];
            obj->unkF7 = st->unk48[1];
            obj->unkF8 = st->unk48[2];
            obj->unkF9 = st->unk48[3];
            obj->unkFA = st->unk48[4];
            obj->unkFB = st->unk48[5];
            obj->unkFC = st->unk4E;
            obj->unkFE = 0;
            obj->flags |= 1;
        }
    } else {
        obj->unk104 = 0;
        obj->unk100 = 0;
        if (st->unk04 == 0x1E) {
            r = (s32 **)data->unk04;
            obj->unk108[0] = r[2] ? *r[2] : 0;
            i = 1;
            off = 0;
            for (; i < 4; i++) {
                s32 val = 0;
                s32 *e = *(s32 **)((u8 *)r + off + 0x18);
                if (e) {
                    val = *e;
                }
                obj->unk108[i] = val;
                off += 8;
            }
        }
    }
    st->unk20 = s7;
    obj->unkDF = 0;
    if (st->unk40 & 1) {
        obj->unkDF = 0x10;
    }
    if (st->unk40 & 2) {
        obj->unkDF |= 8;
    }
    func_800188D0(st->unk38, 0, 0, (u8 *)obj + 0x2C);
    {
        u16 h = st->unk38;
        obj->unkF4 = 0;
        obj->unkEC = h;
        obj->unkF2 = h;
    }
    if ((st->unk40 & 4) || (st->unk40 & 1)) {
        obj->unkE8 = (st->unk3C * 16 / 218) >> 4;
        obj->unkEA = st->unk3E;
        obj->unkDF |= 1;
        obj->unkF0 = 0;
        obj->unkE0 = 0;
        obj->unkDE = st->unk35;
        obj->unkDC = st->unk36;
    }
    if (st->unk40 & 8) {
        obj->unkE8 = (st->unk3C * 16 / 300) >> 4;
        obj->unkF0 = st->unk38;
        obj->unkDF |= 2;
    }
    if (obj->unk104) {
        func_80017668((u8 *)obj->unk104 + 0x28, 0, (u8 *)obj + 0x50);
    } else {
        s32 a;
        s32 v = (s8)st->unk34;
        a = v == 0 ? -1 : v == 1 ? -2 : v - 2;
        obj->unk54 = func_80067684((u8 *)obj + 0x50, a) + (st->unk3A << 4);
    }
    func_80060668(st->unk24, (u8 *)obj + 0xA0);
    if (*(s32 *)((u8 *)obj->unk28 + 0x10)) {
        func_80044988(*(void **)((u8 *)obj->unk28 + 0x10), (u8 *)obj + 0xC8, 1, 0);
    }
    func_80010750((u8 *)obj + 0x2C);
    func_80010234((u8 *)obj->unk28->unk34 + ((obj->unk70 << 4) + 8), (u8 *)obj + 0x5C, 1);
    if (st->unk40 & 0x10) {
        func_8006314C(obj, 0);
        obj->unkDF |= 0x20;
    }
    if (st->unk40 & 0x20) {
        obj->unkDF |= 0x40;
    }
}
