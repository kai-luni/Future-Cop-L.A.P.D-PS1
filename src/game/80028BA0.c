// Categories: Utilities > Memory
#include "../../include/game/types.h"

/**
 * @brief Word-wise block copy between two buffers.
 *
 * The byte count is shifted right by two, so only whole words are copied and a
 * trailing remainder is dropped. Source and destination must be word aligned,
 * overlap is not handled.
 *
 * @param destination Target buffer.
 * @param source Source buffer.
 * @param byte_count Bytes to copy (rounded down to words).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * One counted loop.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes byte_count / 4 words at destination.
 *
 * @par Callers
 * Confirmed: func_80028BCC (chunk prepare).
 */
void func_80028BA0(u32 *destination, const u32 *source, u32 byte_count)
{
    u32 count = byte_count >> 2;

    while (count != 0) {
        *destination++ = *source++;
        count--;
    }
}
