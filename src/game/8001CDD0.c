// Categories: Resources > Memory Management | Platform > CD-ROM
#include "game/types.h"

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");

/**
 * @brief Default mode-2 memory allocator wrapper registered by the file/CD layer.
 *
 * Forwards size with allocation tag 2 (forward next-fit free-range search) to func_80017EBC. Installed into D_8009F230 by func_8001D248 when no custom allocator callback is provided, and called indirectly by func_8001E110.
 *
 * @param size Requested usable byte count passed to func_80017EBC.
 * @return Pointer to the allocated block, or 0 on allocation failure.
 *
 * @par Globals
 * None directly; address is stored in D_8009F230 by func_8001D248.
 *
 * @par Control flow
 * Single call to func_80017EBC(size, 2) and return.
 *
 * @par Calls
 * func_80017EBC - allocates size bytes using forward next-fit free-range mode 2.
 *
 * @par Side effects
 * Allocates heap/free-range memory via func_80017EBC.
 *
 * @par Callers
 * Confirmed: none direct; address taken in func_8001D248 (0x8001D28C) into D_8009F230 and called indirectly by func_8001E110 (0x8001E1F0 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void *func_8001CDD0(s32 size)
{
    return gAllocMemory(size, 2);
}
