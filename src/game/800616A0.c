// Categories: Gameplay > Object Spawning | Game Engine > Frame
#include "game/types.h"

struct UnkSub {
    u8 unk00[0xC];
    u16 unk0C;
    s16 unk0E;
    u16 unk10;
    s16 unk12;
    u16 unk14;
    s16 unk16;
};

struct UnkEntity {
    u8 unk00[4];
    struct UnkSub *sub;
};

struct UnkEntityTable {
    s32 count;
    struct UnkEntity *items[1];
};

extern struct UnkEntityTable *gRegistryTable __asm__("D_8009F800");
extern void gTrySpawnRegisteredUnit(struct UnkEntity *entity, s32 arg) __asm__("func_8006125C");

/**
 * @brief Steps all entity spawners in D_8009F800, draining queued spawn counts and advancing interval timers.
 *
 * Iterates D_8009F800->items[0..count-1]: fires func_8006125C(entity, 1) for unk10 bits 8..11
 * and func_8006125C(entity, 0) for bits 12..15, clears those upper bits of unk10, and when
 * unk16 < unk0E, unk12 != 0 and (unk10 & 1) == 0, ticks timer unk14 down by elapsed and respawns on underflow.
 *
 * @param elapsed Frame delta subtracted from each active spawner timer (unk14); callers pass level id D_8009F9FC.
 *
 * @par Globals
 * D_8009F800 - pointer to UnkEntityTable (count at +0x00, entity pointers at +0x04).
 * @par Control flow
 * Outer loop over D_8009F800->items with two inner burst loops and a gated timer-underflow check.
 * @par Calls
 * func_8006125C - executes queued or timed spawn/stat step (direct jal at 0x80061704, 0x8006172C, and timer path in asm/us/main.s).
 * @par Side effects
 * Clears high bits of sub->unk10, updates sub->unk14, and triggers spawn/stat mutations via func_8006125C.
 * @par Callers
 * Confirmed: func_80027510 (two-player unpaused frame loop); also direct call at 0x80027488 in asm/us/main.s (single-player frame loop).
 */
void func_800616A0(s32 elapsed)
{
    struct UnkEntity **cursor;
    struct UnkEntity *entity;
    struct UnkSub *sub;
    s32 remaining;
    s32 i;
    u16 flags;

    cursor = gRegistryTable->items;
    for (remaining = gRegistryTable->count; remaining > 0; remaining--) {
        entity = *cursor++;
        sub = entity->sub;
        for (i = (sub->unk10 >> 8) & 0xF; i > 0; i--) {
            gTrySpawnRegisteredUnit(entity, 1);
        }
        for (i = sub->unk10 >> 12; i > 0; i--) {
            gTrySpawnRegisteredUnit(entity, 0);
        }
        flags = sub->unk10;
        sub->unk10 = flags & 0xFF;
        if (sub->unk16 < sub->unk0E && sub->unk12 != 0 && (flags & 1) == 0) {
            sub->unk14 -= elapsed;
            if ((s16)sub->unk14 < 0) {
                sub->unk14 += sub->unk0C;
                gTrySpawnRegisteredUnit(entity, 1);
            }
        }
    }
}
