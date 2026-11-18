// Categories: Gameplay > Weapons | Gameplay > Entity Update
#include "game/types.h"

struct Control {
    u8 unk00[0x84];
    s16 unk84[3];
    s16 unk8A[3];
    u8 unk90[3];
};

struct Object {
    u8 unk00[0xAC];
    struct Control *ctl;
};

struct Target;
extern struct Target *gRegistryFind(u8 id) __asm__("func_8006AAEC");
extern s32 func_8006BD48(struct Object *obj, struct Target *tgt, s32 flag);

/**
 * @brief Refills one controller counter slot toward its scaled registry maximum.
 *
 * Looks up the slot's definition record by ctl->unk90[idx], queries its scaled limit via
 * func_8006BD48, and increments ctl->unk84[idx] by step (clamped) or sets it to max if step < 0.
 *
 * @param obj Object whose +0xAC pointer holds the Control block (unk84[3], unk90[3]).
 * @param idx Slot index (0..2) selecting unk84[idx] and registry id unk90[idx].
 * @param step Amount to add when >= 0; negative values jump directly to max.
 * @return 1 if ctl->unk84[idx] was already >= max, 0 if it was increased toward max.
 *
 * @par Globals
 * None directly.
 * @par Control flow
 * Compares ctl->unk84[idx] < max; if below, adds step (clamping at max) when step >= 0, else sets to max, returning 0; otherwise returns 1.
 * @par Calls
 * gRegistryFind - looks up target entry for ctl->unk90[idx]; func_8006BD48 - computes scaled limit.
 * @par Side effects
 * Updates obj->ctl->unk84[idx] when below max.
 * @par Callers
 * Confirmed: func_8002FC90 (pickup/supply), func_80058714 (compound object reset), func_80060A0C (pickup/cheat dispatch).
 */
s32 func_8006BE90(struct Object *obj, s32 idx, s32 step)
{
    struct Control *ctl = obj->ctl;
    struct Target *tgt = gRegistryFind(ctl->unk90[idx]);
    s32 max = func_8006BD48(obj, tgt, 0);

    if (ctl->unk84[idx] < max) {
        if (step >= 0) {
            ctl->unk84[idx] += step;
            if (ctl->unk84[idx] > max) {
                ctl->unk84[idx] = max;
            }
        } else {
            ctl->unk84[idx] = max;
        }
        return 0;
    }
    return 1;
}
