// Categories: Game Engine > State
#include "game/types.h"

struct Unk45164Slot {
    u8 unk00[8];
    s32 key;
};

struct Unk45164Current {
    s32 unk00;
    struct Unk45164Slot *slot;
    s32 key;
};

extern struct Unk45164Current gVolumeLevels __asm__("D_800A1800");

extern u8 gIsTrackedSoundId(s32 id) __asm__("func_80045028");
struct Unk45164Slot *gGetTrackedSoundSource(s32 id) __asm__("func_80045164");


/**
 *  @brief Returns the current slot if an ID check succeeds and its key matches.
 *
 *  Uses the current-slot state after validating the supplied ID, and rejects
 *  the slot when its stored key differs from the current key.
 *  @param id Identifier passed to the validation function.
 *  @return Current slot pointer on success, or null if either check fails.
 *  @par Globals
 *  D_800A1800 supplies the current slot pointer and comparison key.
 *  @par Control flow
 *  Calls the validator first; only on success does it dereference the slot
 *  and compare keys. The slot pointer is not checked for null locally.
 *  @par Calls
 *  func_80045028 validates the ID and returns a nonzero value on success.
 *  @par Side effects
 *  No direct writes; the validation call may have effects.
 *  @par Callers
 *  Confirmed: func_800450A4.
 */
struct Unk45164Slot *gGetTrackedSoundSource(s32 id)
{
    register s32 valid __asm__("$3");
    register struct Unk45164Slot *result __asm__("$2");
    struct Unk45164Current *current;

    valid = gIsTrackedSoundId(id);
    result = 0;
    if (valid != 0) {
        current = &gVolumeLevels;
        if (current->slot->key == current->key) {
            result = current->slot;
        }
    }
    return result;
}
