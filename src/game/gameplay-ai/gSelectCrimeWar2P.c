// Categories: Gameplay > Game Modes
#include "game/types.h"

struct Unk78210Mode {
    u8 unk00[0x0C];
    s32 players;
    u8 unk10[0x4C];
    s32 split;
};

extern struct Unk78210Mode *gGameSession __asm__("D_8009FA94");
extern u8 gSavedCrimeWarProgress __asm__("D_8009F3F1");
extern u8 gCrimeWarProgress __asm__("D_8009F458");
extern u8 gLayoutPointerFlagB __asm__("D_8009F3E7");
extern void *gPendingRowCapture __asm__("D_8009F8C4");
extern s32 gDeferredTransition __asm__("D_8009F8C8");
extern u8 gAudioOptionsMenuMode[] __asm__("D_800A377D");
extern u8 gCrimeWarHubMenuInputMode[] __asm__("D_800A386D");
extern u8 gCwRulesMovieName[] __asm__("D_80012088");

extern void func_800776B8(void);

/**
 *  @brief Initializes mode-related state when the argument is 3.
 *
 *  Sets two players and disables split mode, clears several state bytes,
 *  and conditionally selects a one-time pointer after a setup call.
 *  @param a0 Selector; values other than 3 leave state unchanged.
 *  @par Globals
 *  D_8009FA94 supplies the mode; D_8009F3F1, D_800A377D, D_800A386D,
 *  and D_8009F458 are cleared. D_8009F8C8 is set to 4.
 *  D_8009F3E7 controls whether D_8009F8C4 points to D_80012088 or null.
 *  @par Control flow
 *  Returns unless a0 is 3; the final pointer selection depends on D_8009F3E7.
 *  @par Calls
 *  func_800776B8 runs after the initial state resets.
 *  @par Side effects
 *  Updates the listed globals and mode fields; the call may have further effects.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gSelectCrimeWar2P(s32 a0) __asm__("func_8007A450");

void gSelectCrimeWar2P(s32 a0)
{
    register struct Unk78210Mode *mode __asm__("$3");

    if (a0 != 3) {
        return;
    }
    mode = gGameSession;
    gSavedCrimeWarProgress = 0;
    mode->players = 2;
    mode->split = 0;
    gAudioOptionsMenuMode[0] = 0;
    gCrimeWarHubMenuInputMode[0] = 0;
    __asm__ volatile("" ::: "memory");
    gCrimeWarProgress = 0;
    func_800776B8();
    gDeferredTransition = 4;
    gPendingRowCapture = 0;
    if (gLayoutPointerFlagB == 0) {
        gLayoutPointerFlagB = 1;
        gPendingRowCapture = gCwRulesMovieName;
    }
}
