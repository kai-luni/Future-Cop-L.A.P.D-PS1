// Categories: Resources > Memory Management | Game Engine > Game State > Reset
#include "game/types.h"

extern void gFreeTextureDescriptors(void) __asm__("func_8005A220");

/**
 * @brief Clears the loaded texture/effect descriptor table during subsystem reset.
 *
 * Wrapper invoking func_8005A220, which frees each D_8009F7D8[i] entry block and the D_8009F7D8 array itself, then zeroes D_8009F7D8 and entry count D_8009F7DC.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None directly; func_8005A220 frees and clears D_8009F7D8 and resets D_8009F7DC.
 *
 * @par Control flow
 * Single call to func_8005A220 and return.
 *
 * @par Calls
 * func_8005A220 - frees all D_8009F7D8 descriptor entries and zeroes D_8009F7D8/D_8009F7DC.
 *
 * @par Side effects
 * Frees D_8009F7D8 and its entries via func_8005A220.
 *
 * @par Callers
 * Confirmed: func_80024D14 (0x80024D78, between-mission subsystem reset) / Likely: none / Unknown: none.
 */
void func_8005A290(void)
{
    gFreeTextureDescriptors();
}
