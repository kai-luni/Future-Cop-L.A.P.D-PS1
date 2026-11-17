// Categories: Gameplay > Mode Setup
#include "game/types.h"

struct Unk78210Mode {
    u8 unk00[0x0C];
    s32 players;
    u8 unk10[0x4C];
    s32 split;
};

extern struct Unk78210Mode *gGameSession __asm__("D_8009FA94");
extern u8 D_8009F459;
extern u8 gLayoutPointerFlagA __asm__("D_8009F3E6");
extern void *gPendingRowCapture __asm__("D_8009F8C4");
extern s32 gDeferredTransition __asm__("D_8009F8C8");
extern u8 D_800A377D[];
extern u8 D_800A368B[];
extern u8 D_80012094[];

extern void func_80077708(void);

/**
 *  @brief Configures two-player split mode when the selector is 3.
 *
 *  Updates the current mode and associated state bytes, then selects an
 *  optional data pointer on the first activation.
 *  @param a0 Mode selector; values other than 3 leave state unchanged.
 *  @par Globals
 *  D_8009FA94 supplies the mode; D_800A377D and D_8009F459 are cleared.
 *  D_8009F8C8 and D_800A368B are set to 5; D_8009F8C4 is cleared or
 *  points to D_80012094 when D_8009F3E6 changes from zero to one.
 *  @par Control flow
 *  Returns early unless a0 is 3; the pointer assignment depends on D_8009F3E6.
 *  @par Calls
 *  func_80077708 runs after the initial mode and state updates.
 *  @par Side effects
 *  Changes mode fields and global state, and invokes the helper.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8007A56C(s32 a0)
{
    register struct Unk78210Mode *mode __asm__("$3");

    if (a0 != 3) {
        return;
    }
    mode = gGameSession;
    mode->players = 2;
    mode->split = 1;
    D_800A377D[0] = 0;
    __asm__ volatile("" ::: "memory");
    D_8009F459 = 0;
    func_80077708();
    gDeferredTransition = 5;
    gPendingRowCapture = 0;
    if (gLayoutPointerFlagA == 0) {
        gLayoutPointerFlagA = 1;
        gPendingRowCapture = D_80012094;
    }
    D_800A368B[0] = 5;
}
