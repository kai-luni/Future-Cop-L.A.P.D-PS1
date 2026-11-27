// Categories: Game Engine > State Transitions
#include "game/types.h"

extern u32 gCallGateFlag __asm__("D_8009F6DC");
extern void func_80024630(s32 arg0);
extern void gStartVagmAudioStream(s32 arg0) __asm__("func_8002A16C");
void gRestartVagmStreamOnTimeout(void) __asm__("func_8002A23C");


/**
 *  @brief Runs two state operations when a global flag is nonzero.
 *
 *  Uses D_8009F6DC to gate a pair of calls with fixed arguments.
 *  A zero flag leaves both operations uncalled.
 *  @par Globals
 *  D_8009F6DC is read as the gate.
 *  @par Control flow
 *  If the flag is nonzero, calls the two functions in order.
 *  @par Calls
 *  func_80024630 receives 0; func_8002A16C then receives 1.
 *  @par Side effects
 *  Any effects of the two calls occur only when the flag is nonzero.
 *  @par Callers
 *  Confirmed: func_80022654.
 */
void gRestartVagmStreamOnTimeout(void)
{
    if (gCallGateFlag != 0) {
        func_80024630(0);
        gStartVagmAudioStream(1);
    }
}
