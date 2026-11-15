// Categories: Resources > Spawn Tables | Gameplay > Object Spawning
#include "game/types.h"

struct Unk3A90CEntry {
    u8 unk00[3];
    u8 start;
    u8 count;
    u8 unk05[2];
    u8 flags;
    u8 unk08[0x0C];
};

/**
 * @brief Looks up the range entry that contains a key in a start/count table.
 *
 * @param entry First table entry: +3 the start byte, +4 the count byte,
 *        +7 a flags byte whose bit 0x80 marks the last entry.
 * @param key The value to locate.
 * @return The entry with start <= key < start + count, or 0 when the
 *         terminator is reached first.
 *
 * @par Confirmed
 * Walks consecutive 0x14-byte entries while key - start is outside
 * [0, count); stops without a result at the 0x80 terminator flag.
 *
 * @par Likely role
 * The frame-range selector of the particle spawner func_8003AA94: each
 * entry describes one animation range of the effect, the key is the current
 * frame or type id.
 *
 * @par Unknown
 * What the remaining payload bytes of an entry carry.
 *
 * @par Callers
 * func_8003AA94.
 */
struct Unk3A90CEntry *func_8003A90C(struct Unk3A90CEntry *entry, s32 key)
{
    struct Unk3A90CEntry *result;
    s32 delta;

    result = 0;
    while ((delta = key - entry->start) < 0 || delta >= entry->count) {
        if (entry->flags & 0x80) {
            goto done;
        }
        entry++;
    }
    result = entry;
done:
    return result;
}
