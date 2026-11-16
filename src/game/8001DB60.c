// Categories: Platform > State
#include "game/types.h"
struct Entry { volatile u32 state; u32 pad[3]; s32 flags; u32 pad14; };
extern struct Entry D_800A6D78[];
extern s32 func_8001D90C(s32);

/**
 *  @brief Dispatches an indexed entry operation according to its flags.
 *
 *  Entries with flags 2, 3 or 17 are dispatched directly; flag 1 waits
 *  for the low state byte to leave values 2 and 3 before dispatching.
 *  @param index Index of the entry in the global table.
 *  @return The helper's result for supported flags, or -6 otherwise.
 *  @par Globals
 *  D_800A6D78 contains the indexed volatile state and flags.
 *  @par Control flow
 *  Switches on the flags; the flag-1 path busy-waits without a timeout,
 *  sets bit 0x10 around the helper call, then clears that bit.
 *  @par Calls
 *  func_8001D90C performs the indexed operation for supported flags.
 *  @par Side effects
 *  Temporarily modifies the entry flags on the flag-1 path.
 *  @par Callers
 *  Confirmed: func_80028AB4.
 */
s32 func_8001DB60(s32 index)
{
    s32 result;
    register s32 flags __asm__("$8") = D_800A6D78[index].flags;
    switch (flags) {
    case 2:
    case 3:
    case 17:
        return func_8001D90C(index);
    case 1: {
        register s32 two __asm__("$9") = 2;
        register s32 three __asm__("$8") = 3;
        __asm__("" : "+r"(two), "+r"(three));
        while ((D_800A6D78[index].state & 0xff) == two ||
               (D_800A6D78[index].state & 0xff) == three) {}
        D_800A6D78[index].flags |= 0x10;
        result = func_8001D90C(index);
        D_800A6D78[index].flags &= ~0x10;
        return result;
    }
    default:
        return -6;
    }
}
