// Categories: Resources > Resource Registry | Game Engine > Init
#include "game/types.h"

struct ScriptTimer;

extern struct ScriptTimer *gResourceListHead __asm__("D_8009F5B8");
extern s32 gResourceListTail __asm__("D_8009F5BC");

void gResetResourceRegistry(void) __asm__("func_80019A64");

/**
 * @brief Clears the head and tail of the shared resource registry.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Stores zero into two consecutive GP-relative words and returns.
 *
 * @par Globals
 * Clears gResourceListHead and gResourceListTail.
 *
 * @par Callers
 * Direct calls in the retail listing are from gLoadFrontEndModule,
 * func_80024A24.
 *
 * @par Role and path
 * Front end and mission initialization start a fresh resource registry.
 *
 * @par Unknown
 * The wider lifecycle determines when any former nodes are released.
 */
void gResetResourceRegistry(void)
{
    gResourceListHead = 0;
    gResourceListTail = 0;
}
