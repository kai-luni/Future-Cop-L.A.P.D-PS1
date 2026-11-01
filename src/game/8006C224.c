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

extern EntityList D_800A8F90;
extern s32 func_8004E5F0(Entity *source, Entity *target, s32 maxDist, s32 flags);

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
    for (; i < D_800A8F90.count; i++) {
        entity = D_800A8F90.items[i];
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
