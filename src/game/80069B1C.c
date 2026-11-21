// Categories: Game Engine > State Machine | Gameplay > Entity Script
#include "game/types.h"

struct Unk69B1C {
    u8 unk00[0xE4];
    void *unkE4;
    void *unkE8;
    void *unkEC;
    void *unkF0;
};

extern struct Unk69B1C *gGameSession __asm__("D_8009FA94");
extern u8 gFrameTime[] __asm__("D_8009FAA4");
extern u8 D_8009FAB4[];
extern u8 gScreenResetFlag[] __asm__("D_8009FA70");
extern u8 gEffectId[] __asm__("D_8009FACC");
void gBindGameSessionPointers(void) __asm__("func_80069B1C");


/**
 * @brief Binds four global script-VM counter/state buffer addresses onto the mission state structure D_8009FA94.
 *
 * Confirmed: reads D_8009FA94 and stores pointers D_8009FAA4, D_8009FAB4, D_8009FA70, and D_8009FACC into offsets +0xE4, +0xE8, +0xEC, and +0xF0.
 * Category follows D_8009FA94 mission/VM state initialized by func_80069B58 alongside script variable banks and RNG tables before mission start.
 * Likely: exposes global frame stamp (D_8009FAA4), countdown timer (D_8009FA70), and mission flags (D_8009FACC/D_8009FAB4) through D_8009FA94; Unknown: which VM or HUD reader dereferences +0xE4..+0xF0.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009FA94 - target struct pointer (+0xE4..+0xF0 written); D_8009FAA4, D_8009FAB4, D_8009FA70, D_8009FACC - addresses stored.
 * @par Control flow
 * Linear leaf function; four pointer stores through D_8009FA94 with no branches or loops.
 * @par Calls
 * None.
 * @par Side effects
 * Overwrites D_8009FA94->unkE4, unkE8, unkEC, and unkF0.
 * @par Callers
 * Confirmed: func_80069B58 (0x80069CE0 in asm/us/main.s, during script-VM state initialization) / Likely: none / Unknown: none.
 */
void gBindGameSessionPointers(void)
{
    struct Unk69B1C *object;

    object = gGameSession;
    object->unkE4 = gFrameTime;
    object->unkE8 = D_8009FAB4;
    object->unkEC = gScreenResetFlag;
    object->unkF0 = gEffectId;
}
