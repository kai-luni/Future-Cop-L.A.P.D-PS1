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
extern void gLoadQueuedAssets(void) __asm__("func_80024EC8");

void gAdvanceGameplayScreenCountdown(void) __asm__("func_80027D9C");

/**
 * @brief Advances the active screen countdown and runs the asset dispatch pass.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * When the suspend byte is clear and state is 4, calls func_80029C34 and subtracts frame delta
 * from a positive countdown. On expiry stores -1 and selects state 0x0B or, for session +0x5C
 * equal to 1, stops track 0x27 on both present players and selects 0x0D. Every path calls
 * gLoadQueuedAssets.
 *
 * @par Globals
 * Reads gWasSuspended, gFrameDelta, gGameSession and gPlayerEntities. Updates gScreenResetFlag
 * as a countdown and may set gScreenState.
 *
 * @par Callers
 * Direct calls from gRunGameFrameLoop (func_80028618).
 *
 * @par Role and path
 * The gameplay frame loop processes active-screen expiry before asset dispatch.
 *
 * @par Unknown
 * The session field at +0x5C selects the transition, and its other uses are unresolved.
 */
void gAdvanceGameplayScreenCountdown(void)
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
    gLoadQueuedAssets();
}
