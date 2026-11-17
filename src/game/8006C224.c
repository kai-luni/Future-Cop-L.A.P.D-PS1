// Categories: Gameplay > Target Selection | AI > Target Selection
#include "game/types.h"

typedef struct Entity {
    u8 unk00[0x08];
    s32 unk08;
    u8 unk0C[0x0C];
    u8 unk18;
    u8 unk19[0x0B];
    u32 unk24;
} Entity;

typedef struct EntityList {
    s32 count;
    s32 unk4;
    Entity *items[1];
} EntityList;

extern EntityList gCommandListHead __asm__("D_800A8F90");
extern s32 func_8004E5F0(Entity *source, Entity *target, s32 maxDist, s32 flags);

/**
 * @brief Finds the closest eligible opposing entity in D_800A8F90 within range and cone.
 *
 * Scans D_800A8F90.items[0..count-1], rejecting same-team (unk18), inactive (unk08 <= 0),
 * excluded/self pointers, and flagged (unk24 & 0x20005) entities, then scores survivors
 * via func_8004E5F0 and returns the nearest match within maxDist.
 *
 * @param source Seeker entity whose team (+0x18) and position/facing anchor the query.
 * @param exclude Optional entity pointer to skip (e.g., previously locked target), or null.
 * @param maxDist Exclusive upper bound on planar distance (also initial bestDist).
 * @param flags Facing-cone threshold passed to func_8004E5F0 (negative disables cone check).
 * @return Pointer to the closest qualifying Entity in D_800A8F90, or 0 if none matched.
 *
 * @par Globals
 * D_800A8F90 - active EntityList (count +0x00, items +0x08).
 *
 * @par Control flow
 * Linear scan over D_800A8F90.count entries with five filter skips, func_8004E5F0 scoring, and minimum-distance tracking.
 *
 * @par Calls
 * func_8004E5F0 - computes planar distance and optional facing-cone check, returning -1 on rejection.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_80035B7C (0x80035C58, 0x80035E8C, flyer target search), func_8006C34C (0x8006C398, player lock-on acquisition).
 */
Entity *func_8006C224(Entity *source, Entity *exclude, s32 maxDist, s32 flags)
{
    Entity *best;
    s32 bestDist;
    s32 i;
    Entity *entity;
    s32 result;

    bestDist = maxDist;
    i = 0;
    best = 0;
    for (; i < gCommandListHead.count; i++) {
        entity = gCommandListHead.items[i];
        if (entity->unk18 == source->unk18) {
            continue;
        }
        if (entity->unk08 <= 0) {
            continue;
        }
        if (entity == exclude) {
            continue;
        }
        if (entity == source) {
            continue;
        }
        if (entity->unk24 & 0x20005) {
            continue;
        }
        result = func_8004E5F0(source, entity, maxDist, flags);
        if (result < 0) {
            continue;
        }
        if (result < bestDist) {
            best = entity;
            bestDist = result;
        }
    }
    return best;
}
