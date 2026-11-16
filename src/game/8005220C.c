// Categories: Physics > Collision | Gameplay > Player
#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk5220CCoord {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
};

struct Unk5220CInfo {
    u8 unk00[0xC1];
    s8 playerIndex;
    u8 unkC2[0x19A];
    struct Unk5220CCoord coord;
    struct Vec3 unk290;
};

struct Unk5220CEntity {
    u8 unk00[0x08];
    s32 id;
    u8 unk0C[0x18];
    s32 flags;
    struct Unk5220CCoord coord;
    struct Vec3 unk5C;
    u8 unk68[0x44];
    struct Unk5220CInfo *info;
};

struct Unk5220CEntry {
    s32 id;
    struct Unk5220CEntity *target;
};

struct Unk5220CList {
    u8 count;
    u8 pad[3];
    struct Unk5220CEntry entries[8];
};

extern struct Unk5220CList D_800A8DC0[];
extern s32 D_8009FA0C;
extern struct Unk5220CEntity *D_8009FA58[];

extern s32 func_80052098(struct Unk5220CEntity *self, struct Unk5220CCoord *coordA,
                         struct Unk5220CCoord *coordB, struct Unk5220CEntity *other);

/**
 * @brief Checks player entity collisions against D_800A8DC0 candidates and the rival player.
 *
 * Reads self->info (+0xAC), returns 0 immediately in single-player (D_8009FA0C == 1) when D_800A8DC0[playerIndex].count is 0, mirrors self->coord and self->unk5C into info->coord and info->unk290, tests up to 8 id-matched list entries via func_80052098, and in two-player mode (D_8009FA0C == 2) also tests D_8009FA58[1 - playerIndex] if !(flags & 4).
 *
 * @param self Player entity block (+0x28 coord, +0x5C vec, +0xAC player info with +0xC1 playerIndex).
 * @return Total number of collisions reported by func_80052098.
 *
 * @par Globals
 * D_800A8DC0 - per-player 8-slot candidate collision list; D_8009FA0C - active player count (1 or 2); D_8009FA58 - two-slot player entity array.
 *
 * @par Control flow
 * Early return for empty list in mode 1, counted loop over min(count, 8) entries gated by entry->id == other->id, and conditional rival check in mode 2.
 *
 * @par Calls
 * func_80052098 - tests 3D proximity/bounding collision and triggers hit damage and response.
 *
 * @par Side effects
 * Writes info->coord and info->unk290, and applies hit damage/response through func_80052098.
 *
 * @par Callers
 * Confirmed: func_80053A74 (0x800544F4), func_800549F0 (0x8005563C) / Likely: none / Unknown: none.
 */
s32 func_8005220C(struct Unk5220CEntity *self)
{
    struct Unk5220CInfo *info;
    struct Unk5220CList *list;
    struct Unk5220CCoord *src;
    struct Unk5220CCoord *dst;
    struct Unk5220CEntry *entry;
    struct Unk5220CEntity *other;
    s32 count;
    s32 diff;
    s32 rem;
    s32 result;

    info = self->info;
    list = &D_800A8DC0[info->playerIndex];
    if (list->count == 0 && D_8009FA0C == 1) {
        return 0;
    }

    src = &self->coord;
    dst = &info->coord;
    dst->unk04 = src->unk04;
    dst->unk08 = src->unk08;
    dst->unk0C = src->unk0C;
    dst->unk10 = src->unk10;
    dst->unk14 = src->unk14;
    dst->unk18 = src->unk18;
    dst->unk1C = src->unk1C;
    dst->unk20 = src->unk20;
    dst->unk24 = src->unk24;
    dst->unk28 = src->unk28;
    dst->unk2C = src->unk2C;
    dst->unk30 = src->unk30;
    info->unk290 = self->unk5C;

    entry = list->entries;
    result = 0;
    count = list->count;
    diff = 8 - count;
    rem = ((diff >> 31) & diff) + count;
    for (; rem > 0; rem--, entry++) {
        other = entry->target;
        if (entry->id == other->id) {
            result += func_80052098(self, dst, &other->coord, other);
        }
    }

    if (D_8009FA0C == 2) {
        other = D_8009FA58[1 - info->playerIndex];
        if ((other->flags & 4) == 0) {
            result += func_80052098(self, dst, &other->info->coord, other);
        }
    }

    return result;
}
