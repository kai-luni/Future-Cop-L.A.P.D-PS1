// Categories: Graphics > Renderer > Map Cell Sprite | Graphics > Effects > Object-Attached Effect | Graphics > Culling
#include "game/types.h"

extern int gAttachPartEffect();
extern int gBeamPartUpdate();

struct Model;

struct ModelInstance {
    struct Model *model;
    u8 coord[0x30];
    s32 x;
    s32 y;
    s32 z;
    u8 unk40[8];
    s16 frame;
};

struct Unk402DCOwner {
    u8 unk00[8];
    s32 unk08;
};

struct Unk5BDE4Arg2 {
    u8 unk00[8];
    void *unk08;
};

struct Unk5BDE4Source {
    u16 unk00;
    u8 unk02[0xC];
    u8 unk0E;
    u8 unk0F;
};

struct Unk5BDE4Param {
    u8 unk00[0x44];
    struct ModelInstance *instance;
    u8 part;
    u8 flags;
    u8 pad4[4];
};

struct Unk5BDE4Vec {
    s32 x;
    s32 y;
    s32 z;
};

struct DestinationTriple {
    s32 second;
    s32 zero;
    s32 first;
};

struct Unk5BCD4Node {
    struct Unk5BCD4Node *next;
    void *first;
    void *second;
    s32 unk0C;
    void *unk10;
    s32 unk14;
    s32 unk18;
    u16 unk1C;
    u8 unk1E;
    u8 unk1F;
    struct Unk5BCD4Node *self;
    s32 unk24;
    u8 unk28[2];
    u8 flags;
    u8 unk2B;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
};

struct Unk5BCD4Pool {
    void *storage;
    struct Unk5BCD4Node *active;
    struct Unk5BCD4Node *free;
};

struct ListNode {
    struct ListNode *previous;
    struct ListNode *next;
    struct ListNode *tail;
};

extern s32 gPlayerCount[] __asm__("D_8009FA0C");
extern struct Unk5BCD4Pool gOwnerNodeListA __asm__("D_800A22A0");
extern u8 gSpawnTypeFlags[] __asm__("D_800A6578");
extern u8 gTargetDamageEffectHandler[] __asm__("D_8005B934");

extern void gGetViewGroundAnchor(u32 index, struct DestinationTriple *destination) __asm__("func_80069580");
extern s32 gVectorLength3D(s32 *vector) __asm__("func_8001A100");
extern struct Unk5BCD4Node *gFindActiveEffectByOwner(struct Unk5BCD4Pool *pool, void *target) __asm__("func_8005BCD4");
extern struct ListNode *gTakePoolNode(struct ListNode *node) __asm__("func_8003BEF0");
extern void gTransformPartVertex(struct ModelInstance *instance, s32 part, s32 *out) __asm__("func_80017668");
extern s32 gGetEntitySpawnBank(struct Unk402DCOwner *owner) __asm__("func_8003BF50");
extern s32 gSpawnTypeAtOwner(struct Unk402DCOwner *owner, u8 column, s32 arg2, s32 row, s32 arg4) __asm__("func_800402DC");

struct Locals5BDE4 {
    struct DestinationTriple local;
    u8 pad24[4];
    struct Unk5BDE4Vec diff;
    u8 pad34[4];
    struct Unk5BDE4Param param;
};
void gAttachNearPlayerEffect(struct Unk402DCOwner *owner, s32 arg1, struct Unk5BDE4Arg2 *arg2, struct ModelInstance *instance, u8 part, struct Unk5BDE4Source *in_source) __asm__("func_8005BDE4");


/**
 * @brief Attaches a modelled sprite effect to an object part, but only when the
 *        object is near one of the registered world points.
 *
 * @param owner    Owner object used as the pool key, as the row source through
 *                 func_8003BF50 and as the identity passed to func_800402DC.
 * @param arg1     Value stored unchanged in the node at +0x0C.
 * @param arg2     Optional configuration; its field at +0x08 is copied into the node
 *                 at +0x10.
 * @param instance Model instance whose world position sits at +0x34/+0x38/+0x3C and
 *                 whose coordinate block starts at +0x04.
 * @param part     Model part index, sign extended; negative means "no part".
 * @param in_source Effect definition: id at +0x00, byte at +0x0E, column at +0x0F.
 *
 * @return Nothing.
 *
 * @par Confirmed control flow
 * 1. Every entry of the point table (count in D_8009FA0C[0]) is fetched with
 *    func_80069580(i, &triple), which fills the triple from D_800A3190 with a zero
 *    middle component. The distance from the instance position to that point is
 *    computed by func_8001A100 and, when it is 0x1FFFF or less, the local flag is
 *    set to 1.
 * 2. Nothing happens unless that flag survives as non-zero; the effect is therefore
 *    suppressed for objects far away from every registered point.
 * 3. func_8005BCD4(&D_800A22A0, owner) looks up an existing node. Otherwise one is
 *    taken from the pool with func_8003BEF0 (a null result returns), then +0x08 =
 *    owner, +0x0C = arg1, +0x04 = arg2, +0x14 = owner->+0x08 when the owner exists,
 *    +0x10 = arg2->+0x08 when the configuration exists.
 * 4. A parameter block is prepared locally: flags 0, plus 0x40 with the instance
 *    (which is then stored at +0x44), plus 0x20 with a non-negative part, in which
 *    case func_80017668 writes the part position to node+0x2C.
 * 5. func_8003BF50(owner) yields the row and the byte at D_800A6578[(column << 2) +
 *    (row << 10)] has bit 2 cleared - unlike gAttachPartEffect, bit 0 is not set here.
 *    node+0x24 receives the address of D_8005B934, node->self = node and +0x18 = 0x32
 *    (a 50 frame lifetime).
 * 6. func_800402DC(owner, column, flags ? &param : 0, row, &node->self) submits the
 *    entry; a return of 0 sets node->flags = 1 and returns, otherwise +0x1C =
 *    source->+0x00, +0x1E = source->+0x0E and flags = 0.
 * 7. When a node was found the refresh path only resets +0x18 = 0x32, +0x04 = arg2
 *    and +0x10 = arg2->+0x08.
 *
 * @par Likely role
 * The proximity-gated variant of gAttachPartEffect: the same map-cell sprite submission,
 * but with a 0x1FFFF squared radius test against a table of world points, which fits
 * lamps, beacons or checkpoint markers that only emit their sprite when the player
 * is close.
 *
 * @par Unknown
 * - The local hit flag is set inside the loop and never cleared, and the loop can run
 *   zero times, so in C its value at the test is not defined by any assignment; the
 *   original almost certainly relied on a register that already held zero.
 * - Whether D_800A3190 holds map points, waypoints or light positions.
 *
 * @par Callers
 * func_8005F6E8 (message 14) and gBeamPartUpdate (definition type 0x0E).
 *
 * @par Environment path
 * Attaches a timed effect node to an owner object, but only if the model
 * instance's world position (+0x34/+0x38/+0x3C) lies within func_8001A100
 * distance 0x1FFFF of one of the gPlayerCount per-view ground anchors
 * returned by gGetViewGroundAnchor (func_80069580); the hit flag 'found' is
 * never initialised in C and relies on the original register state. A node
 * keyed by the owner is looked up in gOwnerNodeListA (D_800A22A0) with
 * func_8005BCD4 and, if absent, taken from the pool with func_8003BEF0,
 * filled (owner, arg1, arg2, optional part position via gTransformPartVertex,
 * callback 0x8005B934 at +0x24, lifetime 0x32 at +0x18), bit 2 of the
 * D_800A6578 flag word for [row][type] is cleared and the object is spawned
 * through gSpawnTypeAtOwner (func_800402DC); an existing node only gets its
 * lifetime and arg2 refreshed. Callers are func_8005F6E8 and gBeamPartUpdate
 * (func_8006B23C), i.e. runtime effect handling, not level construction.
 */
void gAttachNearPlayerEffect(struct Unk402DCOwner *owner, s32 arg1, struct Unk5BDE4Arg2 *arg2,
                   struct ModelInstance *instance, u8 part, struct Unk5BDE4Source *in_source)
{
    register struct Unk5BDE4Source *source __asm__("$21") = in_source;
    struct Locals5BDE4 scratch;
    struct Unk5BCD4Pool *pool;
    struct Unk5BCD4Node *node;
    s32 i;
    s32 row;
    u32 found;

    __asm__("" : : "r"(source) : "$2");
    for (i = 0; i < gPlayerCount[0]; i++) {
        gGetViewGroundAnchor(i, &scratch.local);
        scratch.diff.x = instance->x - scratch.local.second;
        scratch.diff.y = instance->y - scratch.local.zero;
        scratch.diff.z = instance->z - scratch.local.first;
        if (gVectorLength3D((s32 *)&scratch.diff) <= 0x1FFFF) {
            found = 1;
        }
        __asm__ volatile("" ::: "memory");
    }

    if (found & 0xFF) {
        pool = &gOwnerNodeListA;
        node = gFindActiveEffectByOwner(pool, owner);
        if (node == 0) {
            node = (struct Unk5BCD4Node *)gTakePoolNode((struct ListNode *)pool);
            if (node == 0) {
                return;
            }

            node->second = owner;
            node->unk0C = arg1;
            node->first = arg2;
            if (node->second != 0) {
                node->unk14 = ((struct Unk402DCOwner *)node->second)->unk08;
            }
            if (arg2 != 0) {
                node->unk10 = arg2->unk08;
            }

            scratch.param.flags = 0;
            if (instance != 0) {
                scratch.param.instance = instance;
                scratch.param.flags = 0x40;
            }

            if ((s8)part >= 0) {
                scratch.param.part = part;
                scratch.param.flags |= 0x20;
                gTransformPartVertex(instance, (s8)part, &node->unk2C);
            }

            row = gGetEntitySpawnBank(owner);
    {
        u8 *p = gSpawnTypeFlags;
        p[(source->unk0F << 2) + (row << 10)] &= 0xFB;
    }

            node->unk24 = (s32)gTargetDamageEffectHandler;
            node->self = node;
            node->unk18 = 0x32;

            if (gSpawnTypeAtOwner(owner, source->unk0F,
                              scratch.param.flags ? (s32)&scratch.param : 0, row,
                              (s32)&node->self) == 0) {
                node->flags = 1;
                return;
            }

            node->unk1C = source->unk00;
            node->unk1E = source->unk0E;
            node->flags = 0;
        } else {
            node->unk18 = 0x32;
            node->first = arg2;
            if (arg2 != 0) {
                node->unk10 = arg2->unk08;
            }
        }
    }
}
