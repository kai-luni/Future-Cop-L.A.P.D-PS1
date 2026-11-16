// Categories: Resources > Memory Management | Graphics > Text
#include "game/types.h"

extern void func_800898E8(void);

/**
 * @brief Releases the active text lookup table during subsystem initialization.
 *
 * Wrapper calling func_800898E8, which frees D_8009F518 (the active text lookup table read by func_80089914 and installed by func_800899A4) via func_800181D4 if non-null and zeroes the pointer.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None directly; func_800898E8 reads, frees, and clears D_8009F518 (0x304($gp)).
 *
 * @par Control flow
 * Single call to func_800898E8 and return.
 *
 * @par Calls
 * func_800898E8 - frees D_8009F518 if non-null and resets it to 0.
 *
 * @par Side effects
 * Frees and clears D_8009F518 via func_800898E8.
 *
 * @par Callers
 * Confirmed: func_800780DC (0x8007811C, per-session subsystem setup) / Likely: none / Unknown: none.
 */
void func_800898C8(void)
{
    func_800898E8();
}
