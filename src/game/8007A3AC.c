// Categories: Game Engine > Front End | Resources > Level Table | Platform > Sound
#include "game/types.h"

struct Unk78210Mode {
    u8 unk00[0x0C];
    s32 players;
    u8 unk10[0x4C];
    s32 split;
};

extern s32 gPlayer1MenuAcceptPhase __asm__("D_800A35CC");
extern struct Unk78210Mode *gGameSession[] __asm__("D_8009FA94");
extern u8 gSavedCrimeWarProgress __asm__("D_8009F3F1");
extern u8 gCrimeWarProgress __asm__("D_8009F458");
extern u8 gLayoutPointerFlagB __asm__("D_8009F3E7");
extern void *gPendingRowCapture __asm__("D_8009F8C4");
extern s32 gDeferredTransition __asm__("D_8009F8C8");
extern u8 gAudioOptionsMenuMode[] __asm__("D_800A377D");
extern u8 gCrimeWarHubMenuInputMode[] __asm__("D_800A386D");
extern u8 gCwRulesMovieName[] __asm__("D_80012088");

extern void func_800776B8(void);
extern s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1) __asm__("func_80046C84");

/**
 * @brief Menu confirm handler for 1-player Crime War, configuring single-screen mode and transition state.
 *
 * Returns immediately unless a0 == 3; when D_800A35CC is 0, sets 1P non-split mode (players = 1, split = 0) in D_8009FA94[0], clears D_8009F3F1/D_8009F458, resets the 8-entry list via func_800776B8, sets D_8009F8C8 = 4 and optional first-time movie D_80012088 ("cwrules.wve"); otherwise plays SE 0x4C. Category Game Engine > Front End | Resources > Level Table | Platform > Sound follows the menu globals, list reset, and sound trigger.
 *
 * @param a0 Menu action code (only 3 = confirm is handled).
 *
 * @par Globals
 * D_800A35CC - lock guard; D_8009FA94 - mode ptr array ([0] players=1, split=0); D_8009F3F1/D_8009F458/D_800A377D/D_800A386D - UI flags; D_8009F3E7/D_8009F8C8/D_8009F8C4/D_80012088 - rules flag, transition 4, movie ptr.
 *
 * @par Control flow
 * Early return for a0 != 3, branch on D_800A35CC == 0 vs else SE trigger, and one-shot check on D_8009F3E7 == 0.
 *
 * @par Calls
 * func_800776B8 - resets selection and binds 8-entry D_8009B724 list; func_80046C84 - plays SE 0x4C when D_800A35CC != 0.
 *
 * @par Side effects
 * Writes mode, front-end selection, transition, and rules-seen globals, or plays SE 0x4C.
 *
 * @par Callers
 * Confirmed: none direct (function pointer in D_8009BBF8 at 0x8009BBFC in asm/us/main.s) / Likely: menu widget dispatch via D_8009BBF8 / Unknown: none.
 */
void gSelectCrimeWar1P(s32 a0) __asm__("func_8007A3AC");

void gSelectCrimeWar1P(s32 a0)
{
    struct Unk78210Mode *mode;

    if (a0 != 3) {
        return;
    }
    if (gPlayer1MenuAcceptPhase == 0) {
        mode = gGameSession[0];
        gSavedCrimeWarProgress = 0;
        mode->players = 1;
        mode->split = 0;
        gAudioOptionsMenuMode[0] = 1;
        gCrimeWarHubMenuInputMode[0] = 1;
        __asm__ volatile("" ::: "memory");
        gCrimeWarProgress = 0;
        func_800776B8();
        gDeferredTransition = 4;
        gPendingRowCapture = 0;
        if (gLayoutPointerFlagB == 0) {
            gLayoutPointerFlagB = 1;
            gPendingRowCapture = gCwRulesMovieName;
        }
    } else {
        gPlayNonpositionalSoundEffect(0x4C, 0);
    }
}
