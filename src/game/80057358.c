// Categories: Gameplay > Player | Gameplay > Weapon Charge
#include "game/types.h"

struct Unk57358Info {
    u8 unk00[0xD0];
    s16 charge;
};

struct Unk57358 {
    u8 unk00[0xAC];
    struct Unk57358Info *info;
};
void gAddCameraShake(struct Unk57358 *object, s32 amount) __asm__("func_80057358");


/**
 * @brief Raises the charge value of an object by a fraction of the amount.
 *
 * The charge stored in the info record (0xD0) is only raised below 0x100. The
 * amount is divided by four and capped at 0xFF before it is added, so one call
 * can at most quarter the supply but never overshoot the 8-bit field.
 * @param object Object whose info record holds the charge.
 * @param amount Requested charge amount.
 * @par Globals
 * None.
 * @par Control flow
 * One gate, one clamp.
 * @par Calls
 * None.
 * @par Side effects
 * Writes the charge value at info + 0xD0.
 * @par Callers
 * Confirmed: func_800578B4/func_800578E4 family (weapon pickup paths).
 */
void gAddCameraShake(struct Unk57358 *object, s32 amount)
{
    struct Unk57358Info *info;
    s32 charge;

    info = object->info;
    charge = info->charge;
    if (charge < 0x100) {
        amount /= 4;
        if (amount >= 0x100) {
            amount = 0xFF;
        }
        charge += amount;
        info->charge = charge;
    }
}
