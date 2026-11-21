// Categories: Game Engine > Cheat Codes | Utilities > Hashing
#include "game/types.h"
u32 gHashCheatCode(u32 state) __asm__("func_80060DD8");


/**
 * @brief Applies a 10-round bit-mixing LFSR/hash transform to a 32-bit input code word.
 *
 * Loops 10 times (i = 9 down to 0), shifting state left by 1 each round and XORing masked bits
 * from positions 14->11, 11->6, 17->15, and 16->0 back into state before returning it.
 *
 * @param state Input 32-bit cheat/code accumulator word.
 * @return Scrambled 32-bit hash word after 10 rounds.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Fixed 10-iteration descending for-loop with straight-line shift and XOR feedback per round.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_80060A0C (0x80060A20, hashes candidate code before matching 13 cheat switch cases) / Likely: none / Unknown: none.
 */
u32 gHashCheatCode(u32 state)
{
    u32 mask20000;
    u32 mask10000;
    s32 i;

    mask20000 = 0x20000;
    mask10000 = 0x10000;
    for (i = 9; i >= 0; i--) {
        state <<= 1;
        state ^= (state & 0x4000) >> 3;
        state ^= (state & 0x800) >> 5;
        state ^= (state & mask20000) >> 2;
        state ^= (state & mask10000) >> 16;
    }
    return state;
}
