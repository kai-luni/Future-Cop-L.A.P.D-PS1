// Categories: Gameplay > Timers
#include "game/types.h"

struct Unk40550Timers {
    u8 unk00[9];
    u8 total;
    u8 first;
    u8 second;
};

struct Unk40550 {
    u8 unk00[0x6C];
    struct Unk40550Timers *timers;
};
void gDrainObjectLightColor(struct Unk40550 *object, s32 amount) __asm__("func_80040550");


/**
 *  @brief Applies an amount to three timer fields in priority order.
 *
 *  The second field is consumed first, followed by the first and then total.
 *  A zero total prevents any changes, even if the other fields are nonzero.
 *  @param object Object containing the timer structure pointer.
 *  @param amount Amount to subtract, carried between fields when necessary.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  Returns when total is zero or a field can absorb the remaining amount.
 *  Otherwise clears exhausted fields and clamps total to zero if insufficient.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the timer structure's second, first, and possibly total fields.
 *  @par Callers
 *  Confirmed: func_80040A00.
 */
void gDrainObjectLightColor(struct Unk40550 *object, s32 amount)
{
    if (object->timers->total == 0) {
        return;
    }
    if (object->timers->second != 0) {
        if (object->timers->second >= amount) {
            object->timers->second -= amount;
            return;
        }
        amount -= object->timers->second;
        object->timers->second = 0;
    }
    if (object->timers->first != 0) {
        if (object->timers->first >= amount) {
            object->timers->first -= amount;
            return;
        }
        amount -= object->timers->first;
        object->timers->first = 0;
    }
    if (object->timers->total != 0) {
        if (object->timers->total < amount) {
            object->timers->total = 0;
        } else {
            object->timers->total -= amount;
        }
    }
}
