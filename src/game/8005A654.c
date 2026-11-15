// Categories: Resources > Slot Table | Graphics > Effects > Object Pool
#include "game/types.h"

struct Unk5A654Sample {
    s16 id;
    u8 rate;
};

extern struct Unk5A654Sample **D_8009F7D8;
extern s32 D_8009F7DC;

/**
 * @brief Linear id lookup in the effect/sample table D_8009F7D8.
 *
 * @param id Table id to find.
 * @return The entry index, or -1 when absent.
 *
 * @par Confirmed
 * Scans i < D_8009F7DC entries comparing D_8009F7D8[i]->id; first match
 * wins.
 *
 * @par Likely role
 * The resolver behind the pool allocator func_8005A6A4: the table maps
 * effect ids to their rate words, the index becomes the complemented
 * slot marker.
 *
 * @par Callers
 * func_8005A6A4.
 */
s32 func_8005A654(s32 id)
{
    s32 i;

    for (i = 0; i < D_8009F7DC; i++) {
        if (D_8009F7D8[i]->id == id) {
            return i;
        }
    }
    return -1;
}
