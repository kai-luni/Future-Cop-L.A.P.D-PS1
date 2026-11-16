// Categories: Platform > Memory Card | BIOS > Events | Platform > Interrupts
#include "game/types.h"

extern void func_8002036C(void);
extern void func_8002059C(void);

/**
 * @brief Releases memory-card BIOS event handles and restores the card interrupt callback lock.
 *
 * Teardown wrapper calling func_8002036C (decrements refcount D_8009AB08->unk8C8 and disables/closes the 8 card events at zero) followed by func_8002059C (decrements lockCount and restores savedCallback at zero).
 *
 * @param void No parameters.
 *
 * @par Globals
 * None directly; callees update D_8009AB08 reference/lock counts and read event handles/savedCallback.
 *
 * @par Control flow
 * Straight-line sequence of two calls; no branches or loops.
 *
 * @par Calls
 * func_8002036C - release BIOS card events; func_8002059C - unlock and restore interrupt callback.
 *
 * @par Side effects
 * Decrements card ref/lock counts in D_8009AB08 and, on zero, closes BIOS events and restores callback via func_80091510.
 *
 * @par Callers
 * Confirmed: func_8007FACC (0x8007FAE8, omitted from work list).
 */
void func_8002078C(void)
{
    func_8002036C();
    func_8002059C();
}
