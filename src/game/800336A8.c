#include "../../include/game/types.h"

struct ReferencedObject {
    u8 pad[8];
    s32 value;
};

struct ObjectReference {
    u8 pad[0xC8];
    struct ReferencedObject *object;
    s32 value;
    u8 pad_D0[0x2A];
    u8 dirty;
};

/**
 * Assigns a non-null target object to an object's reference state.
 *
 * If the supplied target differs from the currently stored reference,
 * replaces the reference and copies the target's 32-bit value at
 * offset 0x08 into the state field at offset 0xCC.
 *
 * Clears the state flag at offset 0xFA whenever a non-null target
 * is supplied, even if the target reference is unchanged.
 *
 * Passing NULL leaves the entire state unchanged.
 *
 * This function only updates reference metadata. It does not
 * directly perform movement, combat, or other target actions.
 *
 * Side effects:
 *   - May update the target pointer at offset 0xC8.
 *   - May update the cached target value at offset 0xCC.
 *   - Clears the flag at offset 0xFA for non-null targets.
 */
void func_800336A8(struct ObjectReference *state,
                   struct ReferencedObject *object)
{
    if (object != 0) {
        if (state->object != object) {
            state->object = object;
            state->value = object->value;
        }
        state->dirty = 0;
    }
}
