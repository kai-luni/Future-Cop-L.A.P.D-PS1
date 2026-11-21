// Categories: Gameplay > Target Selection | Gameplay > Player
#include "game/types.h"

extern int gWeaponSlotInput();

typedef struct Unk6C34CControl Unk6C34CControl;

typedef struct Entity {
    u8 unk00[0x08];
    s32 unk08;
    u8 unk0C[0xA0];
    Unk6C34CControl *control;
} Entity;

struct Unk6C34CControl {
    u8 unk00[0x68];
    s32 unk68;
    Entity *unk6C;
    Entity *unk70;
};

extern Entity *gFindNearestOpposingEntity(Entity *source, Entity *exclude, s32 maxDist, s32 flags) __asm__("func_8006C224");
void gAcquirePlayerLockOnTarget(Entity *object, s32 arg1) __asm__("func_8006C34C");


/**
 * @brief Acquires or cycles a target entity for the player's lock-on state.
 *
 * Reads the player control block at object->control (+0xAC), uses func_8006C224
 * to search D_800A8F90 within 0x18000 units and facing threshold 0xF9EE, and
 * stores the selected target pointer (+0x6C) and its unk08 value (+0x68).
 *
 * @param object Player entity pointer whose +0xAC holds Unk6C34CControl.
 * @param arg1 Mode byte (cast to u8): 0 excludes current unk6C to cycle; non-zero keeps it unless null.
 *
 * @par Globals
 * None directly; callee func_8006C224 scans D_800A8F90.
 *
 * @par Control flow
 * Copies control->unk6C to control->unk70 when (u8)arg1 == 0 or unk6C == 0, queries func_8006C224, and on non-null result updates unk6C/unk68 and clears unk70 if (u8)arg1 != 0.
 *
 * @par Calls
 * func_8006C224 - finds nearest eligible opposing entity inside range 0x18000 and cone 0xF9EE.
 *
 * @par Side effects
 * Writes control->unk68, control->unk6C, and control->unk70.
 *
 * @par Callers
 * Confirmed: gWeaponSlotInput (0x80055E78, arg1=0 on fire press), func_800588D4 (0x80058B74, arg1=1 when unk6C==0).
 */
void gAcquirePlayerLockOnTarget(Entity *object, s32 arg1)
{
    Unk6C34CControl *control;
    Entity *result;

    control = object->control;
    if (((u8)arg1) == 0 || control->unk6C == 0) {
        control->unk70 = control->unk6C;
    }
    result = gFindNearestOpposingEntity(object, control->unk70, 0x18000, 0xF9EE);
    if (result != 0) {
        control->unk6C = result;
        control->unk68 = result->unk08;
        if (((u8)arg1) != 0) {
            control->unk70 = 0;
        }
    }
}
