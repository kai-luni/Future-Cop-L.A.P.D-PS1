// Categories: Game Engine > Frame | Gameplay > Entity Script
#include "game/types.h"

struct Unk3F570Node {
    struct Unk3F570Node *next;
    u8 unk04[0x14];
    void *unk18;
    u8 unk1C[0x08];
    void (*unk24)(s32, struct Unk3F570Node *, s32, s32);
};

extern struct Unk3F570Node *gListHeads[7] __asm__("D_800A1070");
extern struct Unk3F570Node *gDeferredSpawnList __asm__("D_8009F30C");
extern struct Unk3F570Node *gPendingSpawnList __asm__("D_8009F314");

extern s32 gUpdateSpawnNodeLifecycle(struct Unk3F570Node *object, s32 arg1) __asm__("func_8003F2AC");
extern void gPrependSpawnNodeByKind(struct Unk3F570Node *object) __asm__("func_8003B44C");
extern void gSyncAttachedNodes(void) __asm__("func_8003D240");
extern void gRunSceneListPassesOnScratch(s32 arg) __asm__("func_8003F530");
extern void gStreamDeferredSpawns(void) __asm__("func_8003FF7C");
extern void gUpdateDebrisOwners(struct Unk3F570Node *object) __asm__("func_8004C2B8");
extern void gDispatchSpawnSlotTicks(struct Unk3F570Node *node, s32 arg) __asm__("func_8003F428");
extern void gUpdateWorldEffects(s32 object) __asm__("func_8005E0AC");
void gRunEntityWorldTick(s32 arg0) __asm__("func_8003F570");


/**
 * @brief The entity world's per-frame tick: effects, transform sync, the
 *        seven type lists, the proximity gate and the retire queue.
 *
 * @param arg0 Frame delta.
 * @return Nothing.
 *
 * @par Confirmed
 * func_8005E0AC(arg0) drives the effects, func_8003D240 syncs transforms.
 * Each of the seven lists D_800A1070[i] is walked: when func_8003F2AC(node,
 * arg0) & 0xFF the node is unlinked and its handler called with message 10
 * (retire). Then func_8003F530(arg0) (scratchpad trampoline),
 * func_8004C2B8(list 4), func_8003F428(list 5, arg0); with D_8009F30C set
 * func_8003FF7C runs the proximity spawn/despawn gate; finally the
 * D_8009F314 queue drains through func_8003B44C.
 *
 * @par Likely role
 * The main gameplay tick called by the frame driver func_80027510: the
 * seven lists are the entity type buckets, message 10 is the shared retire
 * notification, and the tail queue deferred frees from mid-tick.
 *
 * @par Unknown
 * Which entity types the seven buckets hold.
 *
 * @par Callers
 * func_80027510.
 *
 * @par Environment path
 * The per-frame tick of the entity world, called with the frame delta by both
 * session updates, func_800271A4 (one player) and gRunSessionFrame
 * (func_80027510). It drives the effect system (func_8005E0AC) and the
 * transform sync (func_8003D240), then walks the seven buckets of gListHeads
 * and retires every node whose func_8003F2AC result has the low byte set,
 * unlinking it and sending message 10 to its handler. Afterwards it runs the
 * list update passes through func_8003F530, func_8004C2B8 on bucket 4,
 * func_8003F428 on bucket 5, the proximity gate func_8003FF7C when gDeferredSpawnList
 * is non-empty, and finally moves each object queued in gPendingSpawnList
 * into its bucket with func_8003B44C.
 */
void gRunEntityWorldTick(s32 arg0)
{
    struct Unk3F570Node *node;
    struct Unk3F570Node *next;
    struct Unk3F570Node *prev;
    struct Unk3F570Node *tail;
    s32 i;
    s32 zero;

    gUpdateWorldEffects(arg0);
    gSyncAttachedNodes();

    i = 0;
    do {
        node = gListHeads[i];
        prev = 0;
        if (node != 0) {
            do {
                next = node->next;
                if (gUpdateSpawnNodeLifecycle(node, arg0) & 0xFF) {
                    if (prev != 0) {
                        prev->next = node->next;
                    } else {
                        gListHeads[i] = node->next;
                    }
                    zero = 0;
                    node->next = 0;
                    node->unk24(10, node, zero, zero);
                } else {
                    prev = node;
                }
                node = next;
            } while (node != 0);
        }
        i++;
    } while (i < 7);

    gRunSceneListPassesOnScratch(arg0);
    gUpdateDebrisOwners(gListHeads[4]);
    gDispatchSpawnSlotTicks(gListHeads[5], arg0);
    if (gDeferredSpawnList != 0) {
        gStreamDeferredSpawns();
    }
    while (gPendingSpawnList != 0) {
        tail = gPendingSpawnList->next;
        gPrependSpawnNodeByKind(gPendingSpawnList);
        gPendingSpawnList = tail;
    }
}
