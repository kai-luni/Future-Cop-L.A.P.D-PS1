// Categories: Platform > Streaming Audio | Resources > Reference Counting | Game Engine > Frame
#include "game/types.h"

extern u8 gBitFieldState __asm__("D_8009F6B8");
extern s32 gOutStreamId __asm__("D_8009F6BC");
extern s32 D_8009F6C4;
extern s32 gCallGateFlag __asm__("D_8009F6DC");
extern s32 gGameSuspended __asm__("D_8009FA24");
extern s32 gFrameTickCounter __asm__("D_8009FA2C");
extern u8 D_800A0B90[];

extern void func_80029DD8(void);
extern void func_8002A16C(s32 arg0);
extern void func_8002A408(void);

/**
 * @brief Drains deferred asset releases and handles streaming audio timeout teardown and restart.
 *
 * Always calls func_8002A408 to release queued entries in D_800A0C60; if timeout D_8009F6C4 is nonzero and earlier than tick counter D_8009FA2C, clears D_8009F6C4, tears down stream voices via func_80029DD8, clears D_8009F6B8/D_8009F6BC when bit 0x40 is set, and restarts playback via func_8002A16C(1) when D_8009FA24 == 0 and D_8009F6DC is 0 or 2. Category Platform > Streaming Audio | Resources > Reference Counting | Game Engine > Frame reflects the stream state flags, ref-count drain, and frame tick deadline.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F6C4/D_8009FA2C - stream deadline and frame tick; D_8009F6B8/D_8009F6BC/D_8009F6DC/D_8009FA24/D_800A0B90 - stream flags, handle, mode, inhibit, trigger byte.
 *
 * @par Control flow
 * Unconditional queue drain followed by guarded deadline check and nested state/mode conditionals.
 *
 * @par Calls
 * func_8002A408 - drains D_800A0C60 queue; func_80029DD8 - stops stream voices; func_8002A16C - triggers D_800A0B70 transition.
 *
 * @par Side effects
 * Drains D_800A0C60, clears D_8009F6C4, resets D_8009F6B8/D_8009F6BC on bit 0x40, sets D_800A0B90[0] = 1, and restarts stream.
 *
 * @par Callers
 * Confirmed: func_800462D0 (0x80046374 in asm/us/main.s; omitted from work list due to prologue offset) / Likely: none / Unknown: none.
 */
void func_8002A26C(void)
{
    func_8002A408();

    if (D_8009F6C4 != 0 && D_8009F6C4 < gFrameTickCounter) {
        D_8009F6C4 = 0;
        func_80029DD8();
        if (gBitFieldState & 0x40) {
            gBitFieldState = 0;
            gOutStreamId = -1;
        }
        if (gGameSuspended == 0) {
            if (gCallGateFlag == 2 || gCallGateFlag == 0) {
                D_800A0B90[0] = 1;
                func_8002A16C(1);
            }
        }
    }
}
