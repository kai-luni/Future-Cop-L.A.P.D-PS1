// Categories: Game Engine > Entry Management
#include "game/types.h"

typedef struct { s32 unused[2]; s32 value; } Entry;
typedef struct { s32 count; Entry **items; } Table;
extern Table gSoundSystem __asm__("D_800A07A8");
extern s32 func_80021898(Entry *, s32, s32, s32);
extern void func_800219EC(Entry *, s32);

/**
 *  @brief Initialize an entry and register it in an available table slot.
 *
 *  A negative slot argument searches for the first null entry; otherwise the
 *  supplied index is used directly. Initialization must return a positive amount.
 *  @param entry Entry to initialize and register.
 *  @param arg1 First argument forwarded to the initialization calculation.
 *  @param arg2 Slot index, or a negative value to search for a free slot.
 *  @param arg3 Second argument forwarded to the initialization calculation.
 *  @param arg4 Third argument forwarded to the initialization calculation.
 *  @return Slot index on success, -1 if the search exceeds the count,
 *  -2 if the selected slot is occupied, or -3 if initialization fails.
 *  @par Globals
 *  D_800A07A8 supplies the item table and search count.
 *  @par Control flow
 *  Optionally scans for a null slot, then checks occupancy and initialization.
 *  @par Calls
 *  func_80021898 calculates the amount; func_800219EC applies it to the entry.
 *  @par Side effects
 *  On success, writes entry->value and stores the entry pointer in the table.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_80021CB0(Entry *entry, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
{
    Entry **cursor = gSoundSystem.items;
    s32 index = 0;
    s32 result;
    s32 amount;
    if (arg2 < 0) {
        while (*cursor++) {
            index++;
            if (index > gSoundSystem.count)
                { result = -1; goto done; }
        }
    } else {
        index = arg2;
    }
    result = -2;
    if (gSoundSystem.items[index])
        goto done;
    amount = func_80021898(entry, arg1, arg3, arg4);
    if (amount <= 0) { result = -3; goto done; }
    entry->value = amount;
    func_800219EC(entry, amount);
    gSoundSystem.items[index] = entry;
    result = index;
done:
    return result;
}
