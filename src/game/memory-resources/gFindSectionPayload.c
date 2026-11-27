// Categories: Resources > Asset Loader | Resources > File Access
#include "game/types.h"

u8 *gFindSectionPayload(u8 *arg0, u32 arg1, u32 arg2, s32 arg3) __asm__("func_80019C38");

/**
 * @brief Scans a chunked resource for a section ID and tag and returns the
 * corresponding payload address.
 *
 * @param arg0 Start of the blob.
 * @param arg1 Blob extent in bytes.
 * @param arg2 Tag to compare at +0.
 * @param arg3 Section ID to compare at +8.
 * @return Matching record +0x0C, or the exhausted cursor +0x0C on a miss. A
 *     zero section ID returns null.
 *
 * @par Confirmed mechanics
 * A zero ID takes the immediate zero-return branch. Otherwise checks the byte
 * offset against the extent, compares words at +8 and +0, and advances using
 * each record's +4 word. The exit computes cursor +0x0C even after exhaustion.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80029E20, gLoadMapZone,
 * func_8003BDA0, gLoadActorDef5D, gLoadActorDef5E, func_80049420,
 * gLoadActorLightNode, gLoadActorDef5A, func_8004D6D4, func_80060838,
 * func_80060E74, gRegistryLoad, gLoadActorDef5C, gLoadActorDef59,
 * func_800740B8, func_80076878, func_80084110.
 *
 * @par Role and path
 * Resource readers use the payload address for tagged level and actor
 * sections.
 *
 * @par Unknown
 * The higher-level resource format determines which requested sections are
 * guaranteed present.
 */
u8 *gFindSectionPayload(u8 *arg0, u32 arg1, u32 arg2, s32 arg3)
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
