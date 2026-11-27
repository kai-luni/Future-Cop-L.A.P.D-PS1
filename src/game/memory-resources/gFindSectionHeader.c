// Categories: Resources > Asset Loader | Resources > File Access
#include "game/types.h"

extern int gRegistryLoad();

u8 *gFindSectionHeader(u8 *arg0, u32 arg1, u32 arg2, s32 arg3) __asm__("func_80019CA0");

/**
 * @brief Scans a chunked resource for a section ID and tag and returns a
 * record header.
 *
 * @param arg0 Start of the blob.
 * @param arg1 Blob extent in bytes.
 * @param arg2 Tag to compare at +0.
 * @param arg3 Section ID to compare at +8.
 * @return Matching header, last examined header on exhaustion, or null if no
 *     header was visited.
 *
 * @par Confirmed mechanics
 * Checks the byte offset, records the current header, compares words at +8 and
 * +0, and advances by +4 on a miss. The return register receives the last
 * inspected header.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80049420, func_8004D6D4,
 * func_80060838, func_80060E74, gRegistryLoad, func_80084110.
 *
 * @par Role and path
 * Asset readers use this lookup when they need the header along with the
 * payload.
 *
 * @par Unknown
 * The return value alone does not distinguish a missing entry from a match at
 * the last record.
 */
u8 *gFindSectionHeader(u8 *arg0, u32 arg1, u32 arg2, s32 arg3)
{
    u8 *p;
    u8 *result;
    s32 found;
    u8 byte_found;

    result = 0;
    p = arg0;
    found = 0;
    while (!found && (u32)(p - arg0) < arg1) {
        result = p;
        byte_found = (*(s32 *)(p + 8) == arg3) && (*(u32 *)result == arg2);
        found = byte_found;
        if (!byte_found) {
            p = result + *(s32 *)(result + 4);
        }
    }
    return result;
}
