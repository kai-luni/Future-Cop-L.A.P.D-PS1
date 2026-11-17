// Categories: Graphics > Camera > Position Lookup | Gameplay > Player
#include "../../include/game/types.h"

struct SourcePair {
    s32 first;
    s32 second;
};

struct DestinationTriple {
    s32 second;
    s32 zero;
    s32 first;
};

extern struct SourcePair gScenePositionPairs[] __asm__("D_800A3190");

/**
 * @brief Copies one stored ground position into a view triple.
 *
 * @param index Entry index into the position pair table D_800A3190.
 * @param destination Out triple: first/second take the pair, the middle
 *        component is zeroed.
 * @return Nothing.
 *
 * @par Confirmed
 * Reads D_800A3190[index].first/.second and writes them to
 * destination->first/.second with destination->zero = 0. No bounds check.
 *
 * @par Likely role
 * The per-player/per-view ground position lookup: the scene traversal
 * func_8003F6E4 uses it as the fallback camera position, and the proximity
 * gate func_8005BDE4 scans D_8009FA0C[0] of these positions to test spawn
 * distance against either player. The zeroed middle component suggests a
 * ground-plane (x, 0, z) triple.
 *
 * @par Unknown
 * Who fills D_800A3190 and how many entries it holds.
 *
 * @par Callers
 * func_8003E984, func_8003F6E4, func_8003FF7C, func_8005BDE4.
 */
void func_80069580(u32 index, struct DestinationTriple *destination)
{
    s32 first;

    first = gScenePositionPairs[index].first;
    destination->zero = 0;
    destination->first = first;
    destination->second = gScenePositionPairs[index].second;
}
