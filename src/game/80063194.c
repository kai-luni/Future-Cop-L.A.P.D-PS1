// Categories: Physics > Collision > Ground Table | Gameplay > Map
#include "game/types.h"

struct Unk631A0 {
    s32 key;
    u8 value;
};

extern s32 gQueueCount __asm__("D_8009F838");
extern struct Unk631A0 gObjectKeyTable[] __asm__("D_800A31A0");

/**
 * @brief Removes an object from the ground/interaction key table D_800A31A0 by
 *        swap-delete.
 *
 * The table is the flat array of 8-byte {object pointer, id byte} entries that the
 * point query func_8006320C walks when it looks for the surface under a position;
 * its count lives in D_8009F838. Removal keeps the array packed by moving the last
 * entry into the hole; when the removed entry is the last one, only its key word is
 * zeroed and the stale id byte stays behind.
 *
 * @param key Object pointer to remove; compared with the entry's first word.
 * @return Nothing.
 *
 * @par Globals
 * D_8009F838 - entry count, decremented on a hit.
 * D_800A31A0 - the entry array.
 *
 * @par Control flow
 * Linear scan up to the count; on the first match decrement, then either copy the
 * last entry into slot i or clear the key, and return. A miss changes nothing.
 *
 * @par Calls
 * None; the search and the move are inline.
 *
 * @par Side effects
 * Mutates D_800A31A0 and D_8009F838, so a table that is being walked must not call
 * this function.
 *
 * @par Callers
 * Confirmed: func_800382B4 (message 13, when flag bit 0x4000 is set) and
 * func_80041C6C; the package also lists func_8002D664, func_8002F7C8 and
 * func_80041B6C.
 */
void func_80063194(s32 key)
{
    s32 i;

    for (i = 0; i < gQueueCount; i++) {
        if (gObjectKeyTable[i].key == key) {
            gQueueCount--;
            if (i < gQueueCount) {
                gObjectKeyTable[i] = gObjectKeyTable[gQueueCount];
            } else {
                gObjectKeyTable[i].key = 0;
            }
            return;
        }
    }
}
