// Categories: Platform > Interrupt Dispatch
#include "game/intc_globals.h"

typedef unsigned short u16;
typedef unsigned int u32;
typedef void (*Callback)(void);
struct State { u16 active, busy; Callback handlers[11]; u16 mask; };
extern struct State gIrqDispatchState __asm__("D_8009CC88");
extern u16 gIrqRepeatMask __asm__("D_8009CCB8");
extern u16 gIrqDispatchBusy __asm__("D_8009CC8A");
extern u16 *gInterruptStatusHalfPtr __asm__("D_8009DD14");
extern u16 *gInterruptMaskPtr __asm__("D_8009DD18");
extern int gIrqStuckCounter __asm__("D_8009DD20");
extern char gIrqInactiveErrFmt[] __asm__("D_80012830");
extern char gIrqStuckErrFmt[] __asm__("D_8001284C");
extern void gBiosPrintf(char *, ...) __asm__("func_8008E000");
extern void gBiosReturnFromException(void) __asm__("func_80091B60");
void gDispatchIrqs(void) __asm__("func_800916EC");


/**
 *  @brief Dispatches enabled pending interrupt handlers and checks for persistent status.
 *
 *  The routine marks dispatch busy, acknowledges each selected interrupt bit,
 *  and invokes its registered handler before checking for remaining status.
 *  @par Globals
 *  D_8009CC88 holds active, busy, handler, and mask state; D_8009CCB8 is the repeat mask.
 *  D_8009DD14 and D_8009DD18 point to status and mask registers; D_8009DD20 counts persistence.
 *  D_8009CC8A is cleared on exit; D_80012830 and D_8001284C supply diagnostic formats.
 *  @par Control flow
 *  An inactive state is reported first; up to eleven bits are scanned per pass.
 *  Repeated masked status is drained, and persistent unmasked status triggers a reset.
 *  @par Calls
 *  func_8008E000 emits diagnostics; func_80091B60 is called for inactive state and on exit.
 *  Non-null registered handlers are invoked indirectly.
 *  @par Side effects
 *  Writes interrupt status, busy state, and the persistence counter; may invoke handlers.
 *  @par Callers
 *  Confirmed: func_80091614.
 *
 * @par Role and path
 * Dispatches enabled pending PS1 hardware interrupts by scanning bits 0..10
 * of D_8009CC88.mask & *D_8009DD14 & *D_8009DD18, acknowledging each bit in
 * *D_8009DD14 and calling its registered handler while repeating on
 * D_8009CCB8. If an interrupt stays pending for more than 0x800 passes in
 * D_8009DD20, it logs via func_8008E000 and clears *D_8009DD14, then clears
 * D_8009CC8A and calls func_80091B60. Driven by func_80091614.
 */
void gDispatchIrqs(void)
{
    register struct State *state __asm__("$17") = &gIrqDispatchState;
    u16 pending;
    register u16 mask __asm__("$3");
    register u16 *pa1 __asm__("$4");
    register u16 *pb1 __asm__("$2");
    register int i __asm__("$17");
    register Callback *handler __asm__("$18");
    register Callback *base __asm__("$20");
    register int one __asm__("$19");
    __asm__("" : "+r"(state));
    if (!state->active) {
        u16 v;
        v = *gInterruptStatusHalfPtr;
        __asm__ volatile ("" : : "r"(v));
        gBiosPrintf(gIrqInactiveErrFmt, v);
        gBiosReturnFromException();
    }
    pa1 = gInterruptStatusHalfPtr;
    mask = state->mask;
    __asm__ volatile ("" : "+r"(mask));
    state->busy = 1;
    __asm__ volatile ("" : : : "memory");
    pb1 = gInterruptMaskPtr;
    __asm__ volatile ("" : "+r"(pa1), "+r"(pb1));
    if ((pending = mask & *pa1 & *pb1)) {
    one = 1;
    base = state->handlers;
    do {
        i = 0;
        handler = base;
        while (pending && i < 11) {
            if (pending & 1) {
                *gInterruptStatusHalfPtr = ~(one << i);
                if (*handler) (*handler)();
            }
            handler++;
            pending >>= 1;
            i++;
        }
    } while ((pending = gIrqRepeatMask & *gInterruptStatusHalfPtr & *gInterruptMaskPtr));
    }
    {
    register u16 *pa __asm__("$5");
    register u16 *pb __asm__("$6");
    pa = gInterruptStatusHalfPtr;
    pb = gInterruptMaskPtr;
    __asm__("" : "+r"(pa), "+r"(pb));
    if (*pa & *pb) {
        int *cnt;
        __asm__("lui %0,%%hi(D_8009DD20)\n\taddiu %0,%0,%%lo(D_8009DD20)"
                : "=r"(cnt));
        if ((*cnt)++ > 0x800) {
            {
            char *msg;
            __asm__("lui %0,%%hi(D_8001284C)\n\taddiu %0,%0,%%lo(D_8001284C)"
                    : "=r"(msg));
            __asm__ volatile ("" : : "r"(msg));
            gBiosPrintf(msg, *pa, *pb);
            }
            gIrqStuckCounter = 0;
            *gInterruptStatusHalfPtr = 0;
        }
    } else gIrqStuckCounter = 0;
    }
    __asm__ volatile (".set\tnoreorder\n\t"
                      "lui\t$1,%%hi(D_8009CC8A)\n\t"
                      "jal\tfunc_80091B60\n\t"
                      "sh\t$0,%%lo(D_8009CC8A)($1)\n\t"
                      ".set\treorder"
                      : : : "$1", "$31", "memory");
}
