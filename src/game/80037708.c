#include "game/types.h"

struct Obj;
struct Link { u8 pad[0x18]; u8 kind; };
struct Data { u8 pad0[8]; struct Link *link; u16 flags; u8 pad0e[6]; u16 id; u8 type; };
struct Sub { u8 pad[0x34]; u8 *data; };
struct Obj {
    u8 pad0[0x18]; u8 kind; u8 pad19; s16 val1a; s16 val1c;
    u8 pad1e[0xa]; struct Sub *sub;
    u8 pad2c[0x24]; u8 pos[12]; u8 mat[0x14]; s16 val70;
    u8 pad72[0x4a]; s32 valbc; u8 padc0[8]; struct Data *other;
};
extern void func_80037064(struct Obj *, struct Data *, s32);
extern void func_800371FC(struct Obj *, struct Data *);
extern void func_80010750(void *);
extern void func_80010234(void *, void *, s32);
extern s32 func_8002AD58(s32, struct Obj *, struct Data *, s32);
extern s32 func_8002AD58_case9(s32, struct Obj *, struct Data *, s32);
extern void func_800410BC(void *, s32, s32, struct Data *);

s32 func_80037708(s32 msg, struct Obj *obj, struct Data *data, s32 arg3)
{
    switch (msg) {
    case 1:
        return 0xD4;
    case 2:
        func_80037064(obj, data, arg3);
        return 0;
    case 6:
        func_800371FC(obj, data);
        func_80010750((u8 *)obj + 0x2c);
        func_80010234(obj->sub->data + ((obj->val70 << 4) + 8), obj->mat, 1);
        return 0;
    case 10:
        if (obj->other->id == data->id || data->type == 15)
            return 0;
        return 1;
    case 11:
        obj->val1c = -1;
        return 0;
    case 9:
        if (!data->link)
            return 0;
        if ((data->flags & 1) && data->link->kind == obj->kind)
            return 0;
        func_8002AD58_case9(msg, obj, data, arg3);
        return 0;
    case 4:
        func_800410BC(obj->pos, obj->val1a, obj->valbc, obj->other);
        *(s32 *)((u8 *)obj + 0x68) = obj->valbc >> 1;
        func_8002AD58(msg, obj, data, arg3);
        return 0;
    case 5:
        return 0;
    default:
        return func_8002AD58(msg, obj, data, arg3);
    }
}
