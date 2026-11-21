// Categories: Gameplay > Entity Update | Gameplay > Object Spawning
#include "game/types.h"

struct Unk4AADParent {
    u8 unk00[0x39];
    u8 flags;
    u16 count;
};

struct Unk4AADNode {
    u8 unk00[0x28];
    u8 flags;
    u8 unk29[0x1F];
    struct Unk4AADNode *next;
    struct Unk4AADParent *parent;
    u8 unk50[8];
};

struct Unk4AADOwner {
    u8 unk00[0x84];
    struct Unk4AADNode *head;
};

struct Unk4C2B8Sub3 {
    u8 unk00[0x0C];
    u32 flags;
};

struct Unk4C2B8Sub2 {
    struct Unk4C2B8Sub3 *unk00;
};

struct Unk4C2B8Data {
    u8 unk00[0x2C];
    s32 data[9];
};

struct Unk4C2B8Link {
    u8 unk00[0x04];
    struct Unk4C2B8Data *data;
    u8 unk08[0x2C];
    struct Unk4C2B8Sub2 *sub;
    u8 flag38;
};

struct Unk4C2B8Owner {
    struct Unk4C2B8Owner *next;
    u8 unk04[0x08];
    s16 unk0C;
    u8 unk0E[0x08];
    s16 unk16;
    u8 unk18[0x04];
    struct Unk4C2B8Link *volatile link;
    u8 unk20[0x5B];
    u8 flags;
    u8 unk7C[0x04];
    void *unk80;
    struct Unk4AADNode *head;
};

extern s32 gIdentityMatrix[9] __asm__("D_8009AA38");
extern s32 gLiveNodeCount __asm__("D_8009F9DC");

extern void gFreeBlock(void *block) __asm__("func_800181D4");
extern void gCreateModelDebrisEntity(s32 arg0, s32 arg1, s32 arg2, s32 arg3) __asm__("func_8003A8C0");
extern s32 gGuardEffectPoolMessage(s32 arg0, struct Unk4C2B8Owner *arg1, s32 arg2, s32 arg3) __asm__("func_8003C7B8");
extern s32 gHandleObjectEventMessages(s32 message, void *object, s32 arg2, s32 arg3) __asm__("func_8003C980");
extern struct Unk4AADNode *gFreeDebrisFragment(struct Unk4AADOwner *owner, struct Unk4AADNode *previous, struct Unk4AADNode *node) __asm__("func_8004AAD4");
extern s32 gLaunchDebrisObject(struct Unk4C2B8Owner *object, s32 arg1) __asm__("func_8004ABE4");
extern void gSpawnDebrisExpiryEffect(void *object) __asm__("func_8004B180");
extern s32 gSpawnMeshFractureDebris(struct Unk4C2B8Owner *object, s32 arg1) __asm__("func_8004B5F8");
extern void gRemoveExpiredDebrisNodes(struct Unk4C2B8Owner *object) __asm__("func_8004C0C0");
void gUpdateDebrisOwners(struct Unk4C2B8Owner *input) __asm__("func_8004C2B8");


/**
 * @brief Walks a D_800A1070[4] node list and dispatches triggered spawns or child-list sweeps by node->flags.
 *
 * Iterates through `input` via `node->next`: nodes with flag bit `0x08` (`+0x7B`) invoke func_8004B180, nodes with `0x08` clear and `0x10` clear invoke func_8004C0C0, and nodes with `0x10` set (when `0x08` is clear) are skipped. Category Gameplay > Entity Update | Gameplay > Object Spawning follows list-4 tick dispatch from func_8003F570 and spawn/retire callees.
 *
 * @param input Head pointer of the `Unk4C2B8Owner` singly linked list (D_800A1070[4]).
 *
 * @par Globals
 * None directly (caller passes D_800A1070[4]; func_8004C0C0 updates D_8009F9DC).
 *
 * @par Control flow
 * `while (node != 0)` advancing `node = node->next`, branching on `node->flags & 0x08` then `!(node->flags & 0x10)`.
 *
 * @par Calls
 * func_8004B180 - marks node triggered and spawns an object from `shape->position` when `flags & 0x08`.
 * func_8004C0C0 - sweeps child list `unk84`, unlinks expired items, and spawns debris when `!(flags & 0x18)`.
 *
 * @par Side effects
 * Mutates node/child state and spawns entities via func_8004B180 and func_8004C0C0.
 *
 * @par Callers
 * Confirmed: func_8003F570 (0x8003F65C in asm/us/main.s, passing D_800A1070[4]) / Likely: none / Unknown: none.
 */
void gUpdateDebrisOwners(struct Unk4C2B8Owner *input)
{
    register struct Unk4C2B8Owner *node __asm__("$16");
    node = input;
    while (node != 0) {
        u8 flags = node->flags;
        if (flags & 0x08) {
            gSpawnDebrisExpiryEffect(node);
        } else if (!(flags & 0x10)) {
            gRemoveExpiredDebrisNodes(node);
        }
        node = node->next;
    }
}

