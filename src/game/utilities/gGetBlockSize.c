// Categories: Utilities > Memory
#include "game/types.h"
s32 gGetBlockSize(s32 *ptr) __asm__("func_8001E304");


/**
 *  @brief Reads the signed word immediately preceding a pointer.
 *
 *  Returns a sentinel for a null pointer; otherwise reads the preceding
 *  word, which the source identifies as a size.
 *  @param ptr Pointer whose preceding word is read, or null.
 *  @return The preceding word, or -1 if ptr is null.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Branches on whether ptr is null.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None; a non-null ptr must permit reading the preceding word.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Returns the signed 32-bit word immediately preceding a pointer (ptr[-1]) as
 * a block size, or -1 if ptr is NULL. Touches no globals and makes no calls;
 * no direct callers are present in the C tree.
 */
s32 gGetBlockSize(s32 *ptr)
{
    s32 size = -1;

    if (ptr != 0) {
        size = ptr[-1];
    }

    return size;
}
