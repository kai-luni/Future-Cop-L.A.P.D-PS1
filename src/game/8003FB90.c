// Categories: Gameplay > Object Spawning | Game Engine > Object Pool | Resources > Memory Management
#include "game/types.h"

struct UnkFB90Owner {
    u8 unk00[0x14];
    u8 unk14;
};

struct UnkFB90Sub {
    u8 unk00[4];
    void *unk04;
    u8 unk08[0x30];
    u8 unk38;
};

struct UnkFB90Node {
    struct UnkFB90Node *next;
    u8 unk04[0x0B];
    u8 unk0F;
    u8 unk10;
    u8 unk11[0x0B];
    struct UnkFB90Sub *unk1C;
    struct UnkFB90Owner *unk20;
};

struct UnkFB90Global {
    struct UnkFB90Global *next;
    u8 unk04;
    u8 unk05[0x0B];
    void *unk10;
};

extern struct UnkFB90Node *gListHeads[7] __asm__("D_800A1070");
extern struct UnkFB90Global *gDeferredSpawnList __asm__("D_8009F30C");

extern void func_8003CA80(void *object);
extern void gFreeBlock(void *object) __asm__("func_800181D4");
void gRetireEntitiesByOwnerAndKind(void *object_, s32 kind_) __asm__("func_8003FB90");


/**
 * @brief Retires active D_800A1070 nodes and frees deferred D_8009F30C entries matching an owner and kind.
 *
 * Scans all seven D_800A1070 linked lists for entries with node->unk20->unk14 == kind whose owner matches object (via unk1C->unk04 when unk1C && !unk1C->unk38, else object == 0), calling func_8003CA80 if unk10 & 4 or setting unk0F |= 1; then unlinks and frees matching nodes from D_8009F30C. Category Gameplay > Object Spawning | Game Engine > Object Pool | Resources > Memory Management follows the object buckets, deferred queue, and free call.
 *
 * @param object Owner pointer matched against active node->unk1C->unk04 and deferred entry->unk10 (or 0).
 * @param kind Kind byte matched against node->unk20->unk14 and entry->unk04.
 *
 * @par Globals
 * D_800A1070 - 7-bucket array of active spawned object lists; D_8009F30C - singly linked list of deferred spawn entries.
 *
 * @par Control flow
 * 7-iteration bucket loop with nested list traversal, followed by a single-pass linked-list filter/unlink loop over D_8009F30C.
 *
 * @par Calls
 * func_8003CA80 - sends teardown message 7 to node->callback and resets active node; func_800181D4 - frees unlinked D_8009F30C entry.
 *
 * @par Side effects
 * Retires or flags D_800A1070 nodes, unlinks/updates D_8009F30C, and frees matched deferred entries.
 *
 * @par Callers
 * Confirmed: func_8003FD10 (0x8003FD54 in asm/us/main.s, opcode 0x65) / Likely: none / Unknown: none.
 */
void gRetireEntitiesByOwnerAndKind(void *object_, s32 kind_)
{
    register void *object __asm__("$19");
    register s32 kind __asm__("$21");
    register struct UnkFB90Global *previous __asm__("$20");
    struct UnkFB90Node *node;
    struct UnkFB90Global *entry;
    struct UnkFB90Global *next;
    s32 i;

    object = object_;
    kind = kind_;
    previous = 0;
    for (i = 0; i < 7; i++) {
        node = gListHeads[i];
        while (node != 0) {
            if (node->unk20->unk14 == kind &&
                ((node->unk1C != 0 && node->unk1C->unk38 == 0)
                    ? node->unk1C->unk04 == object
                    : object == 0)) {
                if (node->unk10 & 4) {
                    func_8003CA80(node);
                } else {
                    node->unk0F |= 1;
                }
            }
            node = node->next;
        }
    }

    entry = gDeferredSpawnList;
    while (entry != 0) {
        next = entry->next;
        if (entry->unk10 == object && entry->unk04 == kind) {
            if (previous != 0) {
                previous->next = next;
            } else {
                gDeferredSpawnList = next;
            }
            entry->next = 0;
            gFreeBlock(entry);
        } else {
            previous = entry;
        }
        entry = next;
    }
}
