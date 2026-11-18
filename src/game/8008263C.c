// Categories: Game Engine > Front-End Configuration | Resources > Memory Management
#include "game/types.h"

typedef struct {
    u8 unk00[0x1C];
    s32 unk1C;
} Unk8008263C;

extern Unk8008263C **gPublishedBlockPtr __asm__("D_8009F3F4");
extern void func_800181D4(void *block);

/**
 * @brief Files a type-tagged block into the 30-slot registry D_8009F3F4, or
 *        releases it when its slot is already taken.
 *
 * D_8009F3F4 is the 0x78-byte array of 30 pointers allocated and zeroed by
 * func_800780DC, read by func_800826AC and freed by func_8007852C. The block
 * selects its slot through +0x1C, accepted in 1..0x1D (29 usable slots): index
 * 0, 0x1E and above, a missing registry, or an occupied slot all end with the
 * block being freed instead of stored.
 *
 * @param arg Loaded request block; +0x1C is its type/slot index.
 * @return Nothing.
 *
 * @par Globals
 * D_8009F3F4 - 30-pointer registry (0x78 bytes, tag-2 pool).
 *
 * @par Control flow
 * Unsigned range test on +0x1C, null test on the registry, occupancy test, then
 * either the single store or the free.
 *
 * @par Calls
 * func_800181D4 - releases the block when it cannot be registered.
 *
 * @par Side effects
 * Stores one pointer in the registry and transfers ownership to it, or frees the
 * block.
 *
 * @par Callers
 * Confirmed: func_80024EC8 (the request whose tag is 0x436D6463, "Cmdc").
 */
void gRegisterPublishedBlock(Unk8008263C *arg) __asm__("func_8008263C");

void gRegisterPublishedBlock(Unk8008263C *arg)
{
    if ((u32)(arg->unk1C - 1) < 0x1D) {
        if (gPublishedBlockPtr == 0) {
            goto end;
        }
        if (gPublishedBlockPtr[arg->unk1C] != 0) {
            goto call;
        }
        gPublishedBlockPtr[arg->unk1C] = arg;
        goto end;
    }
call:
    func_800181D4(arg);
end:
    ;
}
