// Categories: Gameplay > Target Management
#include "game/types.h"

struct Unk423F0Info {
    u8 unk00[0x300];
    void *target;
};

struct Unk423F0Player {
    u8 unk00[0xAC];
    struct Unk423F0Info *info;
};

struct Unk423F0Object {
    u8 unk00[0xA0];
    s8 side;
};

extern struct Unk423F0Player *D_8009FA58[];

/**
 *  @brief Clears matching player target references to an object.
 *
 *  The object's side selects player slot 0, slot 1, or both slots.
 *  Only existing players with existing info and a matching target are changed.
 *  @param message Unused message argument.
 *  @param object Object whose target references are to be cleared.
 *  @return Always zero.
 *  @par Globals
 *  D_8009FA58 supplies the player pointers to inspect.
 *  @par Control flow
 *  Selects a slot range, then loops over it with null and target checks.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Sets matching player info target pointers to null.
 *  @par Callers
 *  Confirmed: func_800426A8.
 */
s32 func_80042470(s32 message, struct Unk423F0Object *object)
{
    s32 first;
    s32 last;
    s32 i;

    first = 0;
    last = 1;
    if (object->side == 0) {
        last = first;
    } else if (object->side == 1) {
        first = 1;
    }
    for (i = first; i <= last; i++) {
        if (D_8009FA58[i] != 0 && D_8009FA58[i]->info != 0 &&
            D_8009FA58[i]->info->target == object) {
            D_8009FA58[i]->info->target = 0;
        }
    }
    return 0;
}
