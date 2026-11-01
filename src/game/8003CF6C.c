#include "game/types.h"

struct Obj8003CF6C {
    u8 unk00[0x28];
    s32 x;
    s32 y;
    s32 z;
    u8 unk34[0xC];
    s32 unk40;
    u8 unk44[6];
    u8 flags4A;
};

struct Node8003CF6C {
    struct Node8003CF6C *next;
    u8 unk04[0x30];
    struct Obj8003CF6C *obj;
    u8 unk38;
    u8 flags39;
    u8 unk3A[6];
    u16 unk40;
    u8 unk42;
};

extern struct Node8003CF6C *D_8009F320;

extern s32 func_8001A100(s32 *vector);
extern void func_80013AA0(s32 low, s32 high);
extern u8 func_80017508(struct Obj8003CF6C *instance, u32 *flags, s32 arg2);
extern void func_800164F4(struct Obj8003CF6C *obj);

void func_8003CF6C(s32 arg0)
{
    struct Node8003CF6C *node;
    s32 vec[3];
    u32 flags;
    s32 *origin;

    origin = (s32 *)arg0;
    flags = 0;
    for (node = D_8009F320; node != 0; node = node->next) {
        if (node->obj != 0 && (node->flags39 & 2)) {
            vec[0] = node->obj->x - origin[0];
            vec[1] = node->obj->y - origin[1];
            vec[2] = node->obj->z - origin[2];
            if (func_8001A100(vec) - node->obj->unk40 > 0x1FFFF) {
                continue;
            }
            if (node->unk40 != 0) {
                func_80013AA0(((u8 *)node)[0x40], ((u8 *)node)[0x41]);
            }
            func_80017508(node->obj, &flags, node->unk42);
            if (node->unk40 != 0) {
                func_80013AA0(0, 0);
            }
            if (node->obj->flags4A & 0x10) {
                func_800164F4(node->obj);
            }
        }
    }
}
