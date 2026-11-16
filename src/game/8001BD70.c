// Categories: Graphics > Color
#include "game/types.h"

extern u32 func_8001BC44(u32 r, u32 g, u32 b);

/**
 *  @brief Converts three signed input components and passes them to a color routine.
 *
 *  Reads elements 1, 3, and 5 as red, green, and blue; values whose
 *  unsigned representation has bits above the low byte become 255.
 *  @param input Array containing the three components at odd indices.
 *  @return The value returned by the color routine.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  Tests each component independently before calling the color routine.
 *  @par Calls
 *  func_8001BC44 receives the three converted byte components.
 *  @par Side effects
 *  The called routine may modify state; its effects are not shown here.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 */
u32 func_8001BD70(s16 *input)
{
    register s16 *p asm("$6") = input;
    u32 r, g, b;

    r = p[1];
    if (r >> 8) r = 255;
    g = p[3];
    if (g >> 8) g = 255;
    b = p[5];
    if (b >> 8) b = 255;
    return func_8001BC44((u8)r, (u8)g, (u8)b);
}
