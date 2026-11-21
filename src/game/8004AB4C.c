// Categories: Game Engine > Object Pool | Graphics > Effects
#include "game/types.h"

struct Unk4AADNode {
    u8 unk00[0x48];
    struct Unk4AADNode *next;
    u8 unk4C[0x0C];
};

extern struct Unk4AADNode *gFragmentFreeList __asm__("D_8009F798");
extern struct Unk4AADNode *gNodePoolAllocation __asm__("D_8009F79C");
extern s32 gLiveNodeCount __asm__("D_8009F9DC");

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void *func_8008CD20(void *buffer, s32 value, s32 size);

/**
 * @brief Builds the fixed pool of 199 effect/attachment nodes used by the
 *        per-owner node lists.
 *
 * The block comes from the tag-2 pool, is zeroed through func_8008CD20 and then
 * threaded into a free list: every node's next pointer sits at +0x48 and the
 * stride is 0x58. The head and a duplicate of it are published in D_8009F798 and
 * D_8009F79C, and the live-node counter is cleared. Nodes are taken by the
 * spawners, released by func_8004AAD4 and reaped by func_8004C0C0.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Globals
 * D_8009F798 - free-list head; func_8004AAD4 pushes released nodes here.
 * D_8009F79C - a second copy of the head, written here and read nowhere else.
 * D_8009F9DC - live-node counter, zeroed here and decremented by func_8004C0C0
 * for every node that dies.
 *
 * @par Control flow
 * Clear the counter, allocate, publish, zero the block, then a counted loop over
 * 0xC7 (199) nodes linking each next pointer. The loop writes the last node's
 * pointer as well, so the chain ends with a pointer past the used area, which
 * nothing follows. No allocation-failure path beyond the null test.
 *
 * @par Calls
 * func_80017EBC - tagged allocation of 0x44C0 bytes, tag 2.
 * func_8008CD20 - fill the block with zero bytes.
 *
 * @par Side effects
 * Allocates and zeroes the pool, links it, writes the three globals.
 *
 * @par Callers
 * Confirmed: func_80024A24 (mission boot).
 */
void func_8004AB4C(void)
{
    struct Unk4AADNode *pool;
    s32 i;

    gLiveNodeCount = 0;
    pool = gAllocMemory(0x44C0, 2);
    gFragmentFreeList = pool;
    gNodePoolAllocation = pool;
    if (pool != 0) {
        func_8008CD20(pool, 0, 0x44C0);
        for (i = 0; i < 0xC7; i++) {
            gFragmentFreeList[i].next = &gFragmentFreeList[i + 1];
        }
    }
}
