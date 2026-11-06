#include "game/types.h"

struct Parent {
    u8 pad[0x34];
    s32 value;
};

struct Object {
    u8 pad[0x24];
    s32 x, y, z;
};

struct Node {
    struct Node *next;
    u8 pad04[0xa];
    s8 type;
    u8 flags;
    u8 pad10[2];
    s16 radius;
    u8 pad14[6];
    u16 param;
    struct Parent *parent;
    u8 pad20[0x14];
    u8 data34[0x55];
    u8 a89, a8a, a8b, a8c, a8d, a8e, a8f;
    u8 pad90[8];
    u16 flags98;
    u8 pad9a[2];
    s32 data9c;
};

extern struct Object *func_8003D9F8(s32, u16, s8, s32, struct Parent *, void *);
extern s32 func_8001A100(s32 *);
extern void func_8006CB58(s32, struct Object *, s32, s32, s32, s32, s32, s32, s32);
extern void func_8006DC24(struct Node *, s32 *);

void func_8006E988(struct Node *node, s32 *origin)
{
    struct Object *object;
    struct Parent *parent;
    s32 arg;
    register s32 mask __asm__("$2");
    s32 vec[3];

    while (node != 0) {
        if (!(node->flags & 8)) {
            if (node->flags98 & 0x40) {
                arg = 0;
                mask = ((u8 *)&node->param)[0] & 4;
                parent = node->parent;
                if (parent != 0) {
                    arg = parent->value;
                }
                object = func_8003D9F8(arg, node->param, node->type, mask, parent, node->data34);
                vec[0] = object->x - origin[0];
                vec[1] = object->y - origin[1];
                vec[2] = object->z - origin[2];
                if (func_8001A100(vec) - (node->radius << 4) <= 0x1ffff) {
                    func_8006CB58(node->data9c, object, node->radius, node->a89, node->a8a, node->a8b, node->a8d, node->a8e, node->a8f);
                }
            } else {
                func_8006DC24(node, origin);
            }
        }
        node = node->next;
    }
}
