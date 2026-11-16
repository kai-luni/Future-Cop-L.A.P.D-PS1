// Categories: Game Engine > State
#include "game/types.h"

extern u32 D_8009F6DC;
extern u32 func_8002443C(void);

/**
 *  @brief Evaluates a state-dependent condition using a threshold when needed.
 *
 *  The global state selects fixed results for values zero, three, and four;
 *  other values require a comparison against a value obtained by a call.
 *  @return Zero for states three or four, one for state zero, or the result
 *  of comparing the called function's value with 0x227 for other states.
 *  @par Globals
 *  D_8009F6DC supplies the state value.
 *  @par Control flow
 *  Checks states three and four first, then zero, then performs the comparison.
 *  @par Calls
 *  func_8002443C supplies the value compared against 0x227.
 *  @par Side effects
 *  No direct writes; the called function may have effects.
 *  @par Callers
 *  Confirmed: func_80029C34.
 */
u32 func_80029BE4(void)
{
    u32 value = D_8009F6DC;

    if (value == 3 || value == 4) {
        return 0;
    }
    if (value == 0) {
        return 1;
    }
    return func_8002443C() >= 0x227;
}
