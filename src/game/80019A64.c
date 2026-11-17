// Categories: Resources > Resource Registry | Game Engine > Init
#include "game/types.h"

struct ScriptTimer;

extern struct ScriptTimer *gResourceList __asm__("D_8009F5B8");
extern s32 gListTailFlag __asm__("D_8009F5BC");

/**
 * @brief Clears the global resource registry head and tail pointers during level initialization.
 *
 * Confirmed: zeroes D_8009F5B8 and D_8009F5BC without walking or freeing nodes. Cross-checking func_80019A74, func_80019AF8, and func_80019BA4 confirms D_8009F5B8 and D_8009F5BC are the head and tail pointers of the doubly-linked resource registry (the local ScriptTimer forward tag in 80019A64.c is an unlinked placeholder name). Category Resources > Resource Registry | Game Engine > Init reflects those globals and the two mission-init callers.
 * Likely: resets the list pointers on a clean mission boot after prior teardown already drained or released resources.
 * Unknown: none.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F5B8 - resource registry list head (cleared to 0). D_8009F5BC - resource registry list tail (cleared to 0).
 *
 * @par Control flow
 * Two unconditional zero stores and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Clears D_8009F5B8 and D_8009F5BC.
 *
 * @par Callers
 * Confirmed: func_80024904 (0x80024958) and func_80024A24 (0x80024BA0), during mission/level initialization / Likely: none / Unknown: none.
 */
void func_80019A64(void)
{
    gResourceList = 0;
    gListTailFlag = 0;
}
