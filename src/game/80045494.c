// Categories: Platform > Sound > SE
#include "game/types.h"

struct Unk7938 {
    u8 unk00[0x10];
    u8 active;
    u8 unk11[0x0B];
};

extern u8 D_800A18D0[];
extern struct Unk7938 D_800A7938[];

/**
 * @brief Resolves a sound id to its active voice-record index across two
 *        id tables.
 *
 * @param id Sound id; >= 0x46 switches to the extended table.
 * @param table Primary id table (caller-supplied, e.g. D_800A1880).
 * @param limit Primary table bound (e.g. 0x45/0x47).
 * @return The record index into D_800A7938, or 0xFF when out of range or
 *         the record is inactive.
 *
 * @par Confirmed
 * Ids >= 0x46 use D_800A18D0 with limit 0x82 and bias -0x46. For 0 < id <
 * limit the table byte selects the record; only records with active != 0
 * are returned, everything else yields 0xFF.
 *
 * @par Likely role
 * The shared SE resolver behind every sound trigger (func_80046C84,
 * func_80047284, the weapon loop func_80046CF0): the two tables split
 * standard and extended effect ids, the active flag gates loaded banks.
 *
 * @par Callers
 * func_80045678, func_80046A58, func_80046C84, func_80046CF0, func_80046E88,
 * func_80047284.
 */
s32 func_80045494(s32 id, u8 *table, s32 limit)
{
    register s32 bias __asm__("$2");
    register s32 adjusted __asm__("$3");
    register u8 *entry __asm__("$2");
    register struct Unk7938 *base __asm__("$3");
    register struct Unk7938 *record __asm__("$2");

    if (id >= 0x46) {
        table = D_800A18D0;
        limit = 0x82;
        bias = -0x46;
    } else {
        bias = 0;
    }
    adjusted = id + bias;
    if (id > 0) {
        if (id < limit) {
            entry = &table[adjusted];
            __asm__ volatile ("" : "+r" (entry));
            base = D_800A7938;
            id = *entry;
            record = &base[id];
            if (record->active != 0) {
                goto done;
            }
        }
    }
    id = 0xFF;
done:
    return id;
}
