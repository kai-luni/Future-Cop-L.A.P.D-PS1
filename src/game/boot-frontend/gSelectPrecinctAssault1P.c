// Categories: Game Engine > Front End | Resources > Level Table | Platform > Sound
#include "game/types.h"

struct Unk78210Mode {
    u8 unk00[0x0C];
    s32 players;
    u8 unk10[0x4C];
    s32 split;
};

extern s32 gPlayer1MenuAcceptPhase __asm__("D_800A35CC");
extern struct Unk78210Mode *gGameSession __asm__("D_8009FA94");
extern u8 gAudioOptionsMenuMode[] __asm__("D_800A377D");
extern u8 gPrecinctAssaultProgress __asm__("D_8009F459");
extern u8 gLayoutPointerFlagA __asm__("D_8009F3E6");
extern void *gPendingRowCapture __asm__("D_8009F8C4");
extern s32 gDeferredTransition __asm__("D_8009F8C8");
extern u8 gMultiBrfMovieName[] __asm__("D_80012094");
extern u8 D_800A368B;

extern void func_80077708(void);
extern s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1) __asm__("func_80046C84");

/**
 * @brief Menu confirm handler for 1-player Precinct Assault, configuring split-mode state and transition targets.
 *
 * Returns immediately unless a0 == 3; when D_800A35CC is 0, sets 1P split mode (players = 1, split = 1) in D_8009FA94, clears D_8009F459, resets the 5-entry list via func_80077708, sets D_8009F8C8 = 5 with optional first-time movie D_80012094 ("MultiBrf.wve"), and sets byte 0x800A368B = 9; otherwise plays SE 0x4C. Category Game Engine > Front End | Resources > Level Table | Platform > Sound follows the menu state globals, func_80077708 list reset, and sound trigger.
 *
 * @param a0 Menu action code (only 3 = confirm is handled).
 *
 * @par Globals
 * D_800A35CC - lock guard; D_8009FA94 - mode (+0x0C players, +0x5C split); D_800A377D/D_8009F459/D_800A368B - UI bytes; D_8009F3E6/D_8009F8C8/D_8009F8C4/D_80012094 - briefing flag, transition 5, movie ptr.
 *
 * @par Control flow
 * Early return for a0 != 3, branch on D_800A35CC == 0 vs else SE trigger, and one-shot check on D_8009F3E6 == 0.
 *
 * @par Calls
 * func_80077708 - resets selection and binds D_8009B72C list; func_80046C84 - plays SE 0x4C when D_800A35CC != 0.
 *
 * @par Side effects
 * Writes mode, front-end list, transition, and briefing-seen globals, or plays SE 0x4C.
 *
 * @par Callers
 * Confirmed: none direct (function pointer in D_8009BBF8 at 0x8009BC50 in asm/us/main.s) / Likely: menu widget dispatch via D_8009BBF8 / Unknown: none.
 */
void gSelectPrecinctAssault1P(s32 a0) __asm__("func_8007A4CC");

void gSelectPrecinctAssault1P(s32 a0)
{
    struct Unk78210Mode *mode;

    if (a0 != 3) {
        return;
    }
    if (gPlayer1MenuAcceptPhase == 0) {
        mode = gGameSession;
        mode->players = 1;
        mode->split = 1;
        gAudioOptionsMenuMode[0] = 1;
        __asm__ volatile("" ::: "memory");
        gPrecinctAssaultProgress = 0;
        func_80077708();
        gDeferredTransition = 5;
        gPendingRowCapture = 0;
        if (gLayoutPointerFlagA == 0) {
            gLayoutPointerFlagA = 1;
            gPendingRowCapture = gMultiBrfMovieName;
        }
        {
            register s32 b3 __asm__("$3") = 0x800A0000;
            __asm__ volatile("" : "+r"(b3));
            *(u8 *)(b3 + 0x368B) = 9;
        }
    } else {
        gPlayNonpositionalSoundEffect(0x4C, 0);
    }
}
