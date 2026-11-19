// Categories: Game Engine > State
#include "game/sio_globals.h"

typedef struct {
    char pad[10];
    volatile short value;
} State;

extern State *gSio0CtrlClearPtr __asm__("D_8009C804");
int gClearSio0CtrlAndWait(void) __asm__("func_8008DA14");


/**
 *  @brief Clears a global state value and performs a short busy wait.
 *
 *  Writes zero to the volatile value in the pointed-to state, then uses a
 *  volatile local counter to retain the delay loop.
 *  @return Always zero.
 *  @par Globals
 *  D_8009C804 supplies the state whose value is cleared.
 *  @par Control flow
 *  Decrements the counter from 10 until it reaches -1.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes zero to the global state's volatile value.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 *
 * @par Role and path
 * Writes 0 to the volatile halfword at offset +0x0A of gSio0CtrlClearPtr
 * (D_8009C804) and executes a 10-iteration busy-wait loop before returning 0.
 * Installed into IRQ node D_800A3E00 by func_8008D964. Performs no function
 * calls.
 */
int gClearSio0CtrlAndWait(void)
{
    volatile int count[3];
    gSio0CtrlClearPtr->value = 0;
    count[0] = 10;
    while (--count[0] != -1) {
    }
    return 0;
}
