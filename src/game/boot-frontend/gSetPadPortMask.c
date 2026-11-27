// Categories: Input > Port State
#include "game/types.h"

typedef void (*Handler)(s32 port);

extern s32 gPadEndSlot __asm__("D_8009C77C");
extern s32 gPadStartSlot __asm__("D_8009C778");
extern s32 gPadBasePort __asm__("D_8009C75C");
extern s32 gPadPresentFlag __asm__("D_8009C760");
extern Handler gPadResetPortHook __asm__("D_8009C72C");
extern s32 gPadPort0IdleCount[] __asm__("D_800A3B70");
extern s32 gPadPort1IdleCount[] __asm__("D_800A3B74");
s32 gSetPadPortMask(s32 arg) __asm__("func_8008A290");


/**
 *  @brief Updates two port-state bits and returns their previous combination.
 *
 *  A changed combination temporarily disables a global flag while updating
 *  port state and resetting counters for bits set in the requested value.
 *  @param arg Requested state combination, using bits 0 and 1.
 *  @return Previous combination of the two global state values.
 *  @par Globals
 *  D_8009C77C and D_8009C778 hold state; D_8009C760 gates updates.
 *  D_800A3B70 and D_800A3B74 hold counters; D_8009C72C is a handler.
 *  D_8009C75C supplies the base port passed to that handler.
 *  @par Control flow
 *  On a change, each set bit resets its counter and may invoke the handler
 *  if the counter is at least 150; clear bits update state without a reset.
 *  @par Calls
 *  D_8009C72C receives the base port or the base plus 0xF0.
 *  @par Side effects
 *  Updates global state and counters and may invoke the handler.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Computes the previous 2-bit port mask from D_8009C77C and (D_8009C778 == 0)
 * and, if different from arg, temporarily clears gPadPresentFlag while
 * updating D_8009C778 and D_8009C77C. When enabling a bit whose idle counter
 * D_800A3B70 or D_800A3B74 is at least 150, invokes D_8009C72C on the
 * corresponding port in gPadBasePort before clearing the counter and
 * restoring gPadPresentFlag.
 */
s32 gSetPadPortMask(s32 arg)
{
    s32 combined = (gPadEndSlot << 1) | (gPadStartSlot == 0);
    if (combined != arg) {
        gPadPresentFlag = 0;
        if (arg & 1) {
            register s32 *p __asm__("$18");
            s32 t;
            __asm__("lui %0,%%hi(D_800A3B70)\n\taddiu %0,%0,%%lo(D_800A3B70)" : "=r"(p));
            t = p[0];
            gPadStartSlot = 0;
            if (t >= 150) gPadResetPortHook(gPadBasePort);
            p[0] = 0;
        } else gPadStartSlot = 1;
        if (arg & 2) {
            register s32 one __asm__("$3") = 1;
            register s32 *q __asm__("$16");
            s32 t;
            __asm__ volatile("" : : "r"(one));
            q = gPadPort1IdleCount;
            __asm__ volatile("" : "=r"(q) : "0"(q));
            t = q[0];
            gPadEndSlot = one;
            if (t >= 150) gPadResetPortHook(gPadBasePort + 0xF0);
            q[0] = 0;
        } else gPadEndSlot = 0;
        gPadPresentFlag = 1;
    }
    return combined;
}
