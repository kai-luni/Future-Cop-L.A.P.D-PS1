// Categories: Resources > Object Copy | Game Engine > Object Pool
#include "game/types.h"

extern u8 *gSlotSpawnDescs[] __asm__("D_800A0C70");
extern u8 *gAllocMemory(s32 size, s32 type) __asm__("func_80017EBC");
extern void gFreeBlock(void *block) __asm__("func_800181D4");
extern void *memcpy(void *dst, const void *src, u32 n);

/**
 * @brief Rebuilds the 0x1C snapshot of one slot from the source record.
 *
 * The slot index comes from arg0[0x14]. An existing block for that slot is
 * released first, then a new 0x1C block of type 2 is allocated and published
 * in D_800A0C70. The first four bytes are copied from 0x18..0x1B, byte 0x1A is
 * overwritten with arg0[0x15], five four-byte groups from 0x1C..0x2F follow,
 * and the halfword at 0x18 receives the source halfword at 0x16.
 *
 * @param arg0 Source record; index at 0x14, id at 0x15, values from 0x18.
 *
 * @par Globals
 * D_800A0C70 - per-slot pointer table.
 *
 * @par Control flow
 * Free, allocate, then a fixed sequence of copies.
 *
 * @par Calls
 * func_80017EBC - upper allocator. func_800181D4 - free. memcpy - bulk copies.
 *
 * @par Side effects
 * Replaces the slot pointer and fills the new block.
 *
 * @par Callers
 * Likely: func_8003BDA0, func_8003BFD0 (same address block).
 */
void func_8003BC7C(u8 *arg0)
{
    u8 *block;

    if (gSlotSpawnDescs[arg0[0x14]] != 0) {
        gFreeBlock(gSlotSpawnDescs[arg0[0x14]]);
    }

    block = gAllocMemory(0x1C, 2);
    gSlotSpawnDescs[arg0[0x14]] = block;

    block[0] = arg0[0x18];
    block[1] = arg0[0x19];
    block[2] = arg0[0x1A];
    block[3] = arg0[0x1B];
    block[0x1A] = arg0[0x15];

    memcpy(block + 4, arg0 + 0x1C, 4);
    memcpy(block + 8, arg0 + 0x20, 4);
    memcpy(block + 0xC, arg0 + 0x24, 4);
    memcpy(block + 0x10, arg0 + 0x28, 4);
    memcpy(block + 0x14, arg0 + 0x2C, 4);

    *(u16 *)(block + 0x18) = *(u16 *)(arg0 + 0x16);
}
