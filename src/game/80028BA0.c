// Categories: Utilities > Memory
#include "../../include/game/types.h"

void gCopyWholeWords(u32 *destination, const u32 *source, u32 byte_count) __asm__("func_80028BA0");

/**
 * @brief Copies the whole words contained in the requested byte count.
 *
 * @param destination Target buffer.
 * @param source Source buffer.
 * @param byte_count Bytes to copy (rounded down to words).
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Shifts the unsigned byte count right by two, then repeatedly loads from a1, stores to a0 and
 * advances both pointers by four.
 *
 * @par Globals
 * None directly.
 *
 * @par Callers
 * Direct calls from gCreateModuleAssetFromHeader (func_80028BCC), gParseStreamedModuleChunks
 * (func_8002902C).
 *
 * @par Role and path
 * The module parser uses this for aligned copies of asset headers and payload fragments.
 *
 * @par Unknown
 * The callers supply suitable alignment and copy direction.
 */
void gCopyWholeWords(u32 *destination, const u32 *source, u32 byte_count)
{
    u32 count = byte_count >> 2;

    while (count != 0) {
        *destination++ = *source++;
        count--;
    }
}
