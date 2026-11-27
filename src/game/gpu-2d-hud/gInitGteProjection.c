// Categories: Graphics > Display Setup
#include "game/types.h"

extern void gSetGeomOffsetAndH(s32 arg0, s32 arg1, s32 arg2) __asm__("func_80010030");

void gInitGteProjection(void) __asm__("func_80019D60");

/**
 * @brief Sets the GTE projection offset and screen distance to the engine
 * defaults.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Calls gSetGeomOffsetAndH with 0xA0, 0x78, and 0x200, placing the last
 * constant in the call delay slot.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80024A24.
 *
 * @par Role and path
 * Mission graphics setup configures the GTE perspective projection before
 * rendering.
 *
 * @par Unknown
 * The routine has no inputs for alternate viewport settings.
 */
void gInitGteProjection(void)
{
    gSetGeomOffsetAndH(0xA0, 0x78, 0x200);
}
