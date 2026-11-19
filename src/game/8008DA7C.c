// Categories: Input > State
#include "game/intc_globals.h"

typedef struct {
    unsigned int previous;
    unsigned int current;
} InputState;

extern InputState *gInterruptStatusStatePtr __asm__("D_8009C808");
int gCheckIrqBit0Persistent(void) __asm__("func_8008DA7C");


/**
 *  @brief Tests whether input bit zero is set in both sampled states.
 *
 *  Reads the current and previous input words from the shared input state
 *  to identify a bit that remains set across both samples.
 *  @return 1 if bit zero is set in both words, otherwise 0.
 *  @par Globals
 *  D_8009C808 points to the input state containing both words.
 *  @par Control flow
 *  Checks the previous bit only when the current bit is set.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Checks whether bit 0 is set in both the current and previous fields of
 * gInterruptStatusStatePtr (D_8009C808), returning 1 if both are set and 0
 * otherwise. Installed into IRQ node D_800A3E00 by func_8008D964. Performs no
 * function calls and modifies no state.
 */
int gCheckIrqBit0Persistent(void)
{
    if (gInterruptStatusStatePtr->current & 1) {
        register unsigned int previous __asm__("$2") = gInterruptStatusStatePtr->previous & 1;
        __asm__ volatile ("" : "+r" (previous));
        if (previous) {
            return 1;
        }
    }
    return 0;
}
