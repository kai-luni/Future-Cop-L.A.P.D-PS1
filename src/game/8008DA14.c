// Categories: Game Engine > State
#include "game/sio_globals.h"

typedef struct {
    char pad[10];
    volatile short value;
} State;

extern State *gSio0CtrlClearPtr __asm__("D_8009C804");

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
 */
int func_8008DA14(void)
{
    volatile int count[3];
    gSio0CtrlClearPtr->value = 0;
    count[0] = 10;
    while (--count[0] != -1) {
    }
    return 0;
}
