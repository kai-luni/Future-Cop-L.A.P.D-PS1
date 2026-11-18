// Categories: Gameplay > State Transitions
#include "game/types.h"

struct UnkFA94 {
    u8 pad00[0x5C];
    s32 unk5C;
};

extern u8 gWasSuspended __asm__("D_8009F680");
extern s32 gFrameDelta __asm__("D_8009F9FC");
extern s32 gScreenState __asm__("D_8009FA00");
extern s32 gScreenResetFlag __asm__("D_8009FA70");
extern struct UnkFA94 *gGameSession __asm__("D_8009FA94");

/* Incomplete array type keeps this out of small data, so the compiler has to
   materialise the base address with lui/addiu like the retail code does. */
extern u8 *gPlayerEntities[] __asm__("D_8009FA58");

extern void func_80029C34(void);
extern void func_80047088(s32 arg0, void *object);
extern void func_80024EC8(void);

/**
 *  @brief Update a gated countdown and transition the current state on expiry.
 *
 *  When the gate is clear and the state is 4, runs an update and subtracts
 *  the elapsed amount from a positive countdown. Expiry selects state 0xD
 *  or 0xB according to a field in the current mode record.
 *  @par Globals
 *  D_8009F680 gates processing; D_8009FA00 holds the state.
 *  D_8009FA70 is the countdown and D_8009F9FC is its decrement.
 *  D_8009FA94 selects the expiry branch; D_8009FA58 supplies two objects.
 *  @par Control flow
 *  Early branches reach the common tail; expiry may scan two object slots.
 *  @par Calls
 *  func_80029C34 updates the active state; func_80047088 handles each
 *  non-null object with code 0x27; func_80024EC8 runs at the common tail.
 *  @par Side effects
 *  Changes the countdown and possibly the state, and invokes object handlers.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80027D9C(void)
{
    s32 i;
    u8 *object;
    register u8 **slot __asm__("$16");

    if (gWasSuspended != 0) {
        goto tail;
    }

    if (gScreenState != 4) {
        goto tail;
    }

    func_80029C34();

    if (gScreenResetFlag <= 0) {
        goto tail;
    }

    gScreenResetFlag = gScreenResetFlag - gFrameDelta;

    if (gScreenResetFlag > 0) {
        goto tail;
    }

    gScreenResetFlag = -1;

    if (gGameSession->unk5C == 1) {
        for (i = 0, slot = gPlayerEntities; i < 2; i++) {
            object = *slot;

            if (object != 0) {
                func_80047088(0x27, object);
            }
            slot++;
        }

        gScreenState = 0xD;
    } else {
        gScreenState = 0xB;
    }

tail:
    func_80024EC8();
}
