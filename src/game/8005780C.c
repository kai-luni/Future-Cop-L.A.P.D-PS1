// Categories: Game Engine > Input > Edge Detection
#include "game/types.h"

extern int gWeaponSlotInput();

struct InputAction {
    u32 buttons;
    s32 phase;
};

/**
 * @brief Four-phase input action state machine: idle, press edge, held,
 *        release edge.
 *
 * @param buttons Current button mask.
 * @param action Action record: +0 the button mask, +4 the phase.
 * @return Nothing.
 *
 * @par Confirmed
 * Pressed (buttons & mask): phase 2 -> 0 (press edge), any other phase ->
 * 3 (held). Released: phase 3 -> 1 (release edge), any phase except 2 ->
 * 2 (idle); phase 2 stays.
 *
 * @par Likely role
 * The shared button-edge decoder of the frame drivers and camera/menu
 * handlers: consumers test phase 0 for a fresh press and phase 1 for a
 * fresh release, 3 drives repeat actions while held.
 *
 * @par Callers
 * func_800260B8, func_800267C8, func_80027510, gWeaponSlotInput, func_800569EC.
 *
 * @par Environment path
 * Four-phase button decoder: a set bit in the current mask moves the action
 * record to the press edge (phase 0) or to held (phase 3), a released mask
 * moves it to the release edge (phase 1) or back to idle (phase 2), and phase
 * 2 stays put until the button is pressed again. Every frame driver and
 * camera/menu handler runs it on its own two-word action record, and
 * func_800271A4 calls it at the top of its frame (src/game/800271A4.c:120).
 * It is pure input state and has nothing to do with the render path.
 */
void func_8005780C(u32 buttons, struct InputAction *action)
{
    if (buttons & action->buttons) {
        if (action->phase == 2) {
            action->phase = 0;
        } else {
            action->phase = 3;
        }
    } else if (action->phase == 3) {
        action->phase = 1;
    } else if (action->phase != 2) {
        action->phase = 2;
    }
}
