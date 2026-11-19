// Categories: Game Engine > Entry Management
#include "game/types.h"

typedef struct { s32 unused[2]; s32 value; } Entry;
typedef struct { s32 count; Entry **items; } Table;
extern Table gSoundSystem __asm__("D_800A07A8");
extern s32 func_80021898(Entry *, s32, s32, s32);
extern void func_800219EC(Entry *, s32);

s32 gRegisterSoundBank(Entry *entry, s32 arg1, s32 arg2, s32 arg3, s32 arg4) __asm__("func_80021CB0");

/**
 * @brief Initializes a sound bank and installs it in a selected or automatically chosen slot.
 *
 * @param entry Entry to initialize and register.
 * @param arg1 First argument forwarded to the initialization calculation.
 * @param arg2 Slot index, or a negative value to search for a free slot.
 * @param arg3 Second argument forwarded to the initialization calculation.
 * @param arg4 Third argument forwarded to the initialization calculation.
 * @return Slot index on success, -1 if the search exceeds the count, -2 if the selected slot is
 * occupied, or -3 if initialization fails.
 *
 * @par Confirmed mechanics
 * Tests a2 for a negative index, then scans four-byte entries until a null pointer or an index
 * strictly greater than the limit. It calls func_80021898 and, on a positive result, stores the
 * value at entry +0x08, calls func_800219EC and installs the entry.
 *
 * @par Globals
 * Reads the bank index limit and table from gSoundSystem, then writes the selected table cell.
 *
 * @par Callers
 * Direct calls from func_800236FC.
 *
 * @par Role and path
 * Registers a loaded bank for later indexed playback and release.
 *
 * @par Unknown
 * The entry layout beyond the field at +0x08 is established elsewhere.
 */
s32 gRegisterSoundBank(Entry *entry, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
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
