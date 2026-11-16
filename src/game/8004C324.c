// Categories: Game Engine > Entity Lifecycle | Graphics > Effects > Object Pool | Gameplay > Object Dynamics
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
extern s32 gBaseMatrix[9] __asm__("D_8009AA38");
extern s32 D_8009F9DC;
extern void func_800181D4(void *);
extern void func_8003A8C0(s32, s32, struct Owner *, s32);
extern s32 func_8003C7B8(s32, struct Owner *, s32, s32);
extern s32 func_8003C980(s32, struct Owner *, s32, s32);
extern struct Node *func_8004AAD4(struct Owner *, struct Node *, struct Node *);
extern u8 func_8004ABE4(struct Owner *, s32);
extern u8 func_8004B5F8(struct Owner *, s32);

/**
 * @brief Message handler of the debris instance built by func_8004C920: message 0
 *        chooses the burst or the polygon-fracture death, message 10 releases the
 *        node chain and the instance itself.
 *
 * @param message Message id: 0 setup, 1 ignored, 10 teardown, anything else goes to
 *        func_8003C980.
 * @param obj Instance; +0x1C link (its +0x04 data block holds nine words at +0x2C,
 *        its +0x34 sub record and its +0x38 flag byte), +0x0C/+0x16 countdown words,
 *        +0x7B flag byte, +0x80 auxiliary block, +0x84 node list head.
 * @param arg2 Forwarded to the effect routines and to the posted request.
 * @param arg3 Forwarded to the event-flag handler func_8003C980.
 * @return 1 when the deferred fracture request was queued, otherwise 0 - including
 *         the early exit when the packet-pool guard func_8003C7B8 reports that the
 *         requested packet amounts do not fit.
 *
 * @par Confirmed
 * Message 0:
 * func_8003C7B8(message, obj, arg2, arg3) runs first and a non-zero result returns 0
 * (the guard's own doc: it compares the free slots of the 0x34- and 0x1C-byte
 * effect pools plus a 50-packet reserve against obj +0x0A and +0x08). Message 1 then
 * returns 0. In message 0, link = +0x1C and sub = link->+0x34 (a record whose target
 * carries a flag bit 25 at +0x0C):
 * - +0x7B bit 8: return func_8004ABE4(obj, arg2), the documented burst variant
 *   (one motion record plus the configured child effect).
 * - +0x7B bit 0x10 with a sub record whose flag is set and link->+0x38 clear: +0x84
 *   is cleared, the nine words of D_8009AA38 are copied into the data block at
 *   +0x2C, func_8003A8C0(data, -1, obj, arg2) posts a request (handler D_8003B2D0,
 *   arguments data/-1/obj/arg2) through func_8004D4D4, and if the +0x16 word was
 *   zero both +0x0C and +0x16 become 300. Returns 1.
 * - +0x7B bit 0x10 without such a sub record, or the +0x38 flag set: falls into the
 *   same exit as the plain case - +0x7B is masked with 0xE7 (bits 8 and 0x10
 *   dropped) and func_8004B5F8(obj, arg2), the polygon fracture and scatter, returns
 *   the value.
 * - no bit set: return func_8004B5F8(obj, arg2).
 *
 * Message 10:
 * When +0x7B bit 8 was set, +0x80 is freed with func_800181D4. The +0x84 chain is
 * then walked: the global live-node counter D_8009F9DC is decremented per node and
 * each node is handed to func_8004AAD4(obj, 0, node), which unlinks it, pushes it
 * onto D_8009F798 and fixes the parent counters; the walk stops when that call
 * returns a null head. Finally the instance itself is freed with func_800181D4 and
 * 0 is returned.
 *
 * @par Likely role
 * The object-message surface of the fracture/debris instance: func_8004C920 stores
 * this function in the instance's handler word at +0x24, so message 0 is the
 * "spawn the death visual" step and message 10 the single free pass of the debris
 * fragments it produced.
 *
 * @par Unknown
 * What bit 25 of the sub record marks, and what the request handler D_8003B2D0 does
 * with the nine copied matrix words.
 *
 * @par Callers
 * No direct callers in the call graph. The only reference is func_8004C920, which
 * takes its address and writes it into the handler word at +0x24 of the 0x88-byte
 * instance it allocates; the handler factories func_8002AE54, func_80077AD4 and
 * func_8006EB80 and the request tables do not name it, so it is reached only as
 * that instance's message handler.
 */
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
