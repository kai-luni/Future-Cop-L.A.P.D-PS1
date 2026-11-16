// Categories: Input > Action Mapping | Gameplay > Player | Gameplay > Weapons
#include "game/types.h"

struct InputAction {
    u32 buttons;
    s32 phase;
};

struct Unk55E10Control {
    u8 unk00[0x30];
    u32 buttons;
    struct InputAction fire;
    struct InputAction special;
    u8 unk44[8];
    struct InputAction weapon;
    u8 unk54[0x51];
    u8 weaponState;
    u8 unkA6[0x26A];
    s8 locked;
};

struct Unk55E10 {
    u8 unk00[0xAC];
    struct Unk55E10Control *control;
};

extern void func_8005780C(u32 buttons, struct InputAction *action);
extern void func_8006C34C(struct Unk55E10 *object, s32 arg1);
extern void func_8006BA50(struct Unk55E10 *object, s32 arg1);

/**
 * @brief Polls player weapon and target-lock inputs for one frame and dispatches weapon slots.
 *
 * When control->locked (+0x2C8) is clear, updates edge-detection states for special, fire, and weapon actions via func_8005780C, refreshes target lock on a fresh fire press, and triggers weapon slots 0..2 via func_8006BA50.
 *
 * @param object Player entity block whose +0xAC pointer holds the input/weapon control structure.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Returns early if control->locked != 0. Otherwise updates special, fire, and weapon InputAction states, calls func_8006C34C(object, 0) when fire.phase == 0 (press edge), calls func_8006BA50 for slot 0 on buttons & 0x04000000, slot 1 on buttons & 0x00400000, and slot 2 when weapon.phase == control->weaponState.
 *
 * @par Calls
 * func_8005780C (4-phase button edge detector), func_8006C34C (target lock acquisition on fire press), func_8006BA50 (drives weapon slots 0, 1, and 2).
 *
 * @par Side effects
 * Updates action phases in control, target lock fields via func_8006C34C, and weapon counters/projectiles via func_8006BA50.
 *
 * @par Callers
 * Confirmed: func_800588D4 (0x80058BD4, during gameplay screen state 4).
 */
void func_80055E10(struct Unk55E10 *object)
{
    struct Unk55E10Control *control;
    u32 buttons;

    control = object->control;
    if (control->locked != 0) {
        return;
    }
    buttons = control->buttons;
    func_8005780C(buttons, &control->special);
    func_8005780C(control->buttons, &control->fire);
    func_8005780C(control->buttons, &control->weapon);
    if (control->fire.phase == 0) {
        func_8006C34C(object, 0);
    }
    if (buttons & 0x4000000) {
        func_8006BA50(object, 0);
    }
    if (buttons & 0x400000) {
        func_8006BA50(object, 1);
    }
    if (control->weapon.phase == control->weaponState) {
        func_8006BA50(object, 2);
    }
}
