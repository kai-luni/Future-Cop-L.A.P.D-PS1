#include "game/types.h"

struct Src {
    u8 pad00[0x24];
    s16 x24;
    s16 x26;
    s16 x28;
    u8 x2A;
};

struct Ent28 { u8 pad[0x34]; u8 *p34; };

struct Obj {
    u8 pad00[0x28];
    struct Ent28 *p28;
    u8 pad2C[0x24];
    s32 v50;
    s32 v54;
    u8 pad58[0x18];
    s16 v70;
    u8 pad72[0x2E];
    u8 vA0;
};

struct Arg { u8 pad[8]; struct Src *src; };

extern s32 func_8002AD58(s32 a0, struct Obj *a1, struct Arg *a2, s32 a3);
extern s32 func_800188D0(s32 a, s32 b, s32 c, void *d);
extern s32 func_80066F78(void *a, s32 b, s32 c, s32 d);
extern void func_8006314C(struct Obj *a, s32 b);
extern void func_80063194(struct Obj *a);
extern void func_80010750(void *a);
extern void func_80010234(void *a, void *b, s32 c);

s32 func_80041C6C(s32 msg, struct Obj *obj, struct Arg *arg, s32 a3)
{
    struct Src *src;
    s32 r;

    switch (msg) {
    case 1:
        return 0xA4;
    case 2:
        func_8002AD58(2, obj, arg, a3);
        src = arg->src;
        func_800188D0(src->x24, src->x26, 0, (u8 *)obj + 0x2C);
        r = func_80066F78((u8 *)obj + 0x50, 0, 0, 0);
        obj->v54 = r + (src->x28 << 4);
        obj->vA0 = src->x2A;
        func_8006314C(obj, obj->vA0);
        func_80010750((u8 *)obj + 0x2C);
        func_80010234(obj->p28->p34 + ((obj->v70 << 4) + 8), (u8 *)obj + 0x5C, 1);
        return 0;
    case 5:
        func_80063194(obj);
        func_8002AD58(msg, obj, arg, a3);
        return 0;
    case 6:
        return 0;
    case 11:
        func_8002AD58(msg, obj, arg, a3);
        return 0;
    case 3:
    case 4:
    case 7:
    case 8:
    case 9:
    case 10:
    default:
        return func_8002AD58(msg, obj, arg, a3);
    }
}
