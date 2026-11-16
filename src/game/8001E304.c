// Categories: Utilities > Memory
#include "game/types.h"

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
 */
s32 func_8001E304(s32 *ptr)
{
    s32 size = -1;

    if (ptr != 0) {
        size = ptr[-1];
    }

    return size;
}
