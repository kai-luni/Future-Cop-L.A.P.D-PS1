// Categories: Resources > Memory Management | Platform > CD-ROM
#include "game/types.h"

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");

void *gAllocateDefaultCdBuffer(s32 size) __asm__("func_8001CDD0");

/**
 * @brief Allocates a buffer through the default CD file allocator callback.
 *
 * @param size Requested byte count.
 * @return Allocated block, or null on allocation failure.
 *
 * @par Confirmed mechanics
 * Passes the incoming $a0 to gAllocMemory with tag 2 in the call delay slot
 * and preserves its result.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * The retail listing has no direct call or tail jump to this entry.
 *
 * @par Role and path
 * The file layer installs the address in its allocator slot when no custom
 * allocator is supplied. Its open path uses the callback to get a buffer.
 *
 * @par Unknown
 * The caller determines the buffer layout.
 */
void *gAllocateDefaultCdBuffer(s32 size)
{
    return gAllocMemory(size, 2);
}
