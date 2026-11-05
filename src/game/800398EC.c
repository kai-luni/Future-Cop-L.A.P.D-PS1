#include "game/types.h"

struct Child {
    u8 pad00[8];
    s32 id;
    u8 pad0C[0x10];
    s16 value;
    u8 pad1E[2];
    u8 kind;
    u8 pad21[3];
    u32 flags;
    u8 pad28[0xC0];
    u16 state;
};
struct Self {
    u8 pad00[0x1C];
    u16 value;
    u8 pad1E[6];
    u32 flags;
    u8 pad28[0x4B];
    s8 count;
    u8 pad74[0xE4];
    struct Child *child[1];
};
extern s32 func_800397AC(s32, struct Self *, void *, s32);
extern s32 func_8002AD58(s32, void *, void *, s32);
extern s32 func_800382B4(s32, struct Self *, void *, s32);
extern void func_80038A00(struct Child *, void *);
extern void func_8004D958(struct Child *);
extern void func_8003BA50(struct Child *, s32);

s32 func_800398EC(s32 message, struct Self *object, void *data, s32 extra)
{
    register s32 msg __asm__("$19") = message;
    register struct Self *self __asm__("$18") = object;
    register void *arg2 __asm__("$20") = data;
    register s32 arg3 __asm__("$21") = extra;
    register struct Self *p __asm__("$16");
    register s32 i __asm__("$17");
    s32 previous;
    register u32 flag __asm__("$19");

    switch (msg) {
    case 1:
        return 0x168;
    case 2:
        return func_800397AC(msg, self, arg2, arg3);
    case 3:
        if (self->flags & 0x20) return 0;
        func_8002AD58(msg, self, arg2, arg3);
        i = 0;
        if (i < self->count) {
            p = self;
            do {
                if (p->child[0]) func_8002AD58(msg, p->child[0], arg2, arg3);
                p = (struct Self *)((u8 *)p + 4);
                i++;
            } while (i < self->count);
        }
        return 0;
    case 6:
        func_800382B4(msg, self, arg2, arg3);
        i = 0;
        if (i < self->count) {
            p = self;
            do {
                if (p->child[0]) func_80038A00(p->child[0], arg2);
                p = (struct Self *)((u8 *)p + 4);
                i++;
            } while (i < self->count);
        }
        goto update;
    case 5:
        i = 0;
        if (i < self->count) {
            p = self;
            do {
                if (p->child[0]) {
                    if (p->child[0]->id) func_8004D958(p->child[0]);
                    p->child[0] = 0;
                }
                p = (struct Self *)((u8 *)p + 4);
                i++;
            } while (i < self->count);
        }
        return func_800382B4(msg, self, arg2, arg3);
    case 9:
    case 11:
    case 14:
        func_800382B4(msg, self, arg2, arg3);
    update:
        i = 0;
        if (i < self->count) {
            flag = 0x02000000;
            p = self;
            do {
                if (p->child[0] && !(p->child[0]->state & 1)) {
                    if (self->flags & flag) p->child[0]->flags |= flag;
                    else p->child[0]->flags &= 0xFDFFFFFF;
                    if (p->child[0]->kind && !(p->child[0]->flags & 0x40000)) {
                        previous = p->child[0]->value;
                        p->child[0]->value = self->value;
                        func_8003BA50(p->child[0], previous);
                    } else {
                        p->child[0]->value = self->value;
                    }
                }
                p = (struct Self *)((u8 *)p + 4);
                i++;
            } while (i < self->count);
        }
        return 0;
    default:
        return func_800382B4(msg, self, arg2, arg3);
    }
}
