// Categories: Gameplay > Player State
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
 *  @brief Assigns an object as the target for selected players.
 *
 *  The object's side selects player zero, player one, or both; missing
 *  player and info pointers are skipped.
 *  @param object Object to assign as the selected players' target.
 *  @par Globals
 *  D_8009FA58 supplies the two player pointers and their info structures.
 *  @par Control flow
 *  Side 0 selects index 0, side 1 selects index 1, and other values select both.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates the target field of each selected player with valid info.
 *  @par Callers
 *  Confirmed: func_800426A8.
 */
void func_800423F0(struct Unk423F0Object *object)
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
        if (D_8009FA58[i] != 0 && D_8009FA58[i]->info != 0) {
            D_8009FA58[i]->info->target = object;
        }
    }
}
