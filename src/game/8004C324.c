#include "game/types.h"

struct Node { u8 pad[0x48]; struct Node *next; };
struct Sub3 { u8 pad[0x0c]; unsigned int unused:25; unsigned int flag:1; unsigned int rest:6; };
struct Sub2 { struct Sub3 *sub; };
struct Data { u8 pad[0x2c]; s32 data[9]; };
struct Link { u8 pad[4]; struct Data *data; u8 pad08[0x2c]; struct Sub2 *sub; u8 flag38; };
struct Owner {
    u8 pad00[0x0c]; s16 unk0c; u8 pad0e[8]; s16 unk16;
    u8 pad18[4]; struct Link *volatile link; u8 pad20[0x5b];
    u8 flags; u8 pad7c[4]; void *unk80; struct Node *head;
};
extern s32 D_8009AA38[9];
extern s32 D_8009F9DC;
extern void func_800181D4(void *);
extern void func_8003A8C0(s32, s32, struct Owner *, s32);
extern s32 func_8003C7B8(s32, struct Owner *, s32, s32);
extern s32 func_8003C980(s32, struct Owner *, s32, s32);
extern struct Node *func_8004AAD4(struct Owner *, struct Node *, struct Node *);
extern u8 func_8004ABE4(struct Owner *, s32);
extern u8 func_8004B5F8(struct Owner *, s32);

s32 func_8004C324(s32 message, struct Owner *obj, s32 arg2, s32 arg3)
{
    struct Link *link;
    struct Sub2 *sub;
    struct Node *node, *next;
    register s32 *hi __asm__("$3");
    s32 first;
    if (func_8003C7B8(message, obj, arg2, arg3)) return 0;
    if (message == 1) return 0;
    if (message == 0) goto case0;
    if (message == 10) goto case10;
    goto default_case;
case0:
    link = obj->link;
    sub = 0;
    if (link) sub = link->sub;
    if (obj->flags & 8) return func_8004ABE4(obj, arg2);
    if (obj->flags & 0x10) {
        if (sub && sub->sub->flag) {
            hi = (s32 *)0x800A0000;
            if (link->flag38) goto failed;
            obj->head = 0;
            __asm__("lw %0,%%lo(D_8009AA38)(%1)" : "=r"(first) : "r"(hi));
            obj->link->data->data[0] = first;
            __asm__("addiu %0,%1,%%lo(D_8009AA38)" : "=r"(hi) : "r"(hi));
            obj->link->data->data[1] = hi[1];
            obj->link->data->data[2] = hi[2];
            obj->link->data->data[3] = hi[3];
            obj->link->data->data[4] = hi[4];
            obj->link->data->data[5] = hi[5];
            obj->link->data->data[6] = hi[6];
            obj->link->data->data[7] = hi[7];
            obj->link->data->data[8] = hi[8];
            func_8003A8C0((s32)obj->link->data, -1, obj, arg2);
            if (!obj->unk16) { obj->unk0c = 300; obj->unk16 = 300; }
            return 1;
        }
failed:
        obj->flags &= 0xe7;
        return func_8004B5F8(obj, arg2);
    }
    return func_8004B5F8(obj, arg2);
case10:
    if (obj->flags & 8) func_800181D4(obj->unk80);
    node = obj->head;
    while (node) {
        D_8009F9DC--;
        next = node->next;
        if (!func_8004AAD4(obj, 0, node)) break;
        node = next;
    }
    func_800181D4(obj);
    return 0;
default_case:
    return func_8003C980(message, obj, arg2, arg3);
}
