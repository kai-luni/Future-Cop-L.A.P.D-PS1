// Categories: Game Engine > Message Dispatch | Gameplay > Entity Script | Gameplay > Entity Update
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
extern s32 gInitType28Entity(s32, struct Self *, void *, s32) __asm__("func_800397AC");
extern s32 gEntityBaseHandler(s32, void *, void *, s32) __asm__("func_8002AD58");
extern s32 gHandleType5Entity(s32, struct Self *, void *, s32) __asm__("func_800382B4");
extern void gUpdateMountedTurret(struct Child *, void *) __asm__("func_80038A00");
extern void gRetireEntity(struct Child *) __asm__("func_8004D958");
extern void gFlagQuartileChange(struct Child *, s32) __asm__("func_8003BA50");
s32 gHandleType28Entity(s32 message, struct Self *object, void *data, s32 extra) __asm__("func_800398EC");


/**
 * @brief Message dispatcher for entity class 28, managing a container object and up to 4 child entities.
 *
 * Reports size 0x168 on message 1, attaches up to four children on message 2 via func_800397AC, handles draw/free/tick/sync messages across self->child[], and delegates base behavior to func_800382B4 or func_8002AD58.
 * @param message Message id (1 size, 2 spawn, 3 draw, 5 free, 6 tick, 9/11/14 state/flag sync, default base).
 * @param object Container entity (+0x1C s16 value, +0x24 flags, +0x73 child count, +0x158 child pointer array).
 * @param data Payload pointer forwarded to child and base handlers.
 * @param extra Extra parameter forwarded to func_800397AC, func_8002AD58, and func_800382B4.
 * @return 0x168 for message 1, 0 for messages 3/6/9/11/14, or the delegated handler return value.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Switch on message: 1 returns 0x168; 2 delegates to func_800397AC; 3 skips if flags & 0x20 else calls func_8002AD58 on self and children; 5 retires valid children with id via func_8004D958, clears slots, and calls func_800382B4; 6 runs func_800382B4 and func_80038A00 per child then updates children; 9/11/14 run func_800382B4 then sync active children (!(state & 1)): copy flags bit 0x02000000, copy +0x1C, and call func_8003BA50 if kind != 0 and !(flags & 0x40000).
 *
 * @par Calls
 * func_800397AC (spawn children), func_8002AD58 (draw self/children), func_800382B4 (base class-5 handler), func_80038A00 (child tick), func_8004D958 (retire child id), func_8003BA50 (notify child on +0x1C change).
 *
 * @par Side effects
 * Updates child flags/values, clears child pointers on message 5, and dispatches child spawn/retire calls.
 *
 * @par Callers
 * Confirmed: func_80039298 (0x800393F0, messages 11/14 for child type 0x1C). Likely: indirect dispatch via handler returned by func_8002AE54 (0x8002AF40) for type 28.
 */
s32 gHandleType28Entity(s32 message, struct Self *object, void *data, s32 extra)
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
        return gInitType28Entity(msg, self, arg2, arg3);
    case 3:
        if (self->flags & 0x20) return 0;
        gEntityBaseHandler(msg, self, arg2, arg3);
        i = 0;
        if (i < self->count) {
            p = self;
            do {
                if (p->child[0]) gEntityBaseHandler(msg, p->child[0], arg2, arg3);
                p = (struct Self *)((u8 *)p + 4);
                i++;
            } while (i < self->count);
        }
        return 0;
    case 6:
        gHandleType5Entity(msg, self, arg2, arg3);
        i = 0;
        if (i < self->count) {
            p = self;
            do {
                if (p->child[0]) gUpdateMountedTurret(p->child[0], arg2);
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
                    if (p->child[0]->id) gRetireEntity(p->child[0]);
                    p->child[0] = 0;
                }
                p = (struct Self *)((u8 *)p + 4);
                i++;
            } while (i < self->count);
        }
        return gHandleType5Entity(msg, self, arg2, arg3);
    case 9:
    case 11:
    case 14:
        gHandleType5Entity(msg, self, arg2, arg3);
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
                        gFlagQuartileChange(p->child[0], previous);
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
        return gHandleType5Entity(msg, self, arg2, arg3);
    }
}
