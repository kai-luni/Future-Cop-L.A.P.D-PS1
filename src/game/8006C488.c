// Categories: Gameplay > Weapons > Sound | Platform > Sound > Voices
#include "game/types.h"

struct Unk6C488Stats {
    u8 unk00[0x8A];
    s16 unk8A;
};

struct Unk6C488 {
    u8 unk00[0xAC];
    struct Unk6C488Stats *stats;
};

extern void func_80046CF0(struct Unk6C488 *object, s32 state);

/**
 * @brief Selects the normal or the alert state of a looping voice.
 *
 * The stats block hangs off the object at +0xAC and the s16 at +0x8A is one of the three per-part words that func_8005D8F4 maintains; while it stays positive the state 0x33 is requested, otherwise 0x32.
 *
 * @param object Entity whose +0xAC points at the stats block.
 * @return Nothing.
 *
 * @par Globals
 * None directly; func_80046CF0 reaches the voice slots D_800A10E0 and the id table D_800A1830.
 *
 * @par Control flow
 * One signed compare (blez) chooses between the two state constants before the single call.
 *
 * @par Calls
 * func_80046CF0 - resolves the state against its sound table and starts or refreshes the loop in one of the owning player's three voice slots.
 *
 * @par Side effects
 * A voice may be started, retuned or stopped; the stats block is only read. This is the branch that switches a looping effect to its damaged variant and back.
 *
 * @par Callers
 * Confirmed: func_8005D8F4 (0x8005DB7C), the expanding contact field tick.
 */
void func_8006C488(struct Unk6C488 *object)
{
    s32 state;

    state = 0x32;
    if (object->stats->unk8A > 0) {
        state = 0x33;
    }
    func_80046CF0(object, state);
}
