// Categories: Resources > Asset Loader | Resources > File Access
#include "game/types.h"

/**
 * @brief Finds a record by tag and section id in a chunked blob and returns a
 *        pointer to its payload.
 *
 * Records are self describing: tag word at +0x0, byte size at +0x4, id at +0x8,
 * payload from +0xC. The payload still starts with the record's own 12-byte
 * header, which is why callers subtract 4 from the result before handing it to a
 * loader (func_800319D4, func_80076878, func_8004D6D4). Known tags in the callers
 * are 0x74455854 ("tEXT") and 0x74414354 ("tACT").
 *
 * @param arg0 Blob base.
 * @param arg1 Blob length in bytes; the walk stops once the offset reaches it.
 * @param arg2 Tag word, compared with the record's +0x0.
 * @param arg3 Section id, compared with the record's +0x8.
 * @return Payload pointer (record + 0xC). 0 only when arg3 is 0; a failed search
 *         returns the current position + 0xC, not 0.
 *
 * @par Globals
 * None; the blob and its length are arguments.
 *
 * @par Control flow
 * While the offset is below arg1 and the record has not matched, advance by the
 * record's own +0x4 size - a corrupt size therefore walks past the end. The tag
 * test is an equality check on the XOR, the id test a plain compare.
 *
 * @par Calls
 * None; the scan is inline.
 *
 * @par Side effects
 * None observed: no writes at all.
 *
 * @par Callers
 * Confirmed: 15 sites, among them func_800319D4 (map zones), func_8004D6D4
 * (actor definition), func_80049420, func_8006ED78, func_8003BDA0 and
 * func_80041070, plus func_80076878, func_80084110 and eight more.
 */
u8 *func_80019C38(u8 *arg0, u32 arg1, u32 arg2, s32 arg3)
{
    u8 *p;
    s32 found;
    u8 byte_found;

    p = arg0;
    if (arg3 == 0) {
        return 0;
    }

    found = 0;
    while (!found && (u32)(p - arg0) < arg1) {
        byte_found = (*(s32 *)(p + 8) == arg3) && (*(u32 *)p == arg2);
        found = byte_found;
        if (!byte_found) {
            p += *(s32 *)(p + 4);
        }
    }

    return p + 0xC;
}
