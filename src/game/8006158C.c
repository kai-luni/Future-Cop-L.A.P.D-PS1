// Categories: Gameplay > Object Spawning | Gameplay > Entity Script
#include "game/types.h"

struct Unk6158CDef {
    s32 id;
};

struct Unk6158CInfo {
    u8 unk00[0x28];
    struct Unk6158CDef *def;
};

struct Unk6158CEntry {
    s32 unk00;
    struct Unk6158CInfo *info;
};

struct Unk60E28Table {
    s32 count;
    struct Unk6158CEntry *entries[0x80];
};

extern struct Unk60E28Table *gRegistryTable __asm__("D_8009F800");
extern void gApplyRegisteredSpawnerCommand(struct Unk6158CEntry *entry) __asm__("func_800614BC");

/**
 * @brief Looks up a spawner entry by definition id in D_8009F800 and forwards to func_800614BC.
 *
 * Confirmed: walks D_8009F800->entries[0..count - 1] comparing entry->info->def->id (+0x04 -> +0x28 -> +0x00) against id; on the first match, invokes func_800614BC(entry) while leaving $a1/$a2 untouched across the loop (verified in asm/us/main.s) so caller args pass through.
 *
 * @param id Spawner definition id matched against entry->info->def->id.
 *
 * @par Globals
 * D_8009F800 - pointer to spawner registry table (+0x00 count, +0x04 entries[] pointer array).
 *
 * @par Control flow
 * Counted descending loop over remaining = D_8009F800->count with early return after calling func_800614BC on match.
 *
 * @par Calls
 * func_800614BC - updates matching entry->sub->flags (+0x04 -> +0x10) using preserved $a1/$a2 command and mode arguments.
 *
 * @par Side effects
 * Mutates matching spawner state via func_800614BC.
 *
 * @par Callers
 * Confirmed: func_80069D68 (0x8006A484 in asm/us/main.s, script VM opcode 60) / Likely: none / Unknown: none.
 */
void func_8006158C(s32 id)
{
    struct Unk60E28Table *table;
    register struct Unk6158CEntry **cursor __asm__("$7");
    struct Unk6158CEntry *entry;
    s32 remaining;
    register s32 target __asm__("$8");

    target = id;
    table = gRegistryTable;
    cursor = table->entries;
    for (remaining = table->count; remaining > 0; remaining--) {
        entry = *cursor++;
        if (entry->info->def->id == target) {
            gApplyRegisteredSpawnerCommand(entry);
            return;
        }
    }
}
