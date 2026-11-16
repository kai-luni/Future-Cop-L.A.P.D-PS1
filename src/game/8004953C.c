// Categories: Game Engine > Scripting
#include "game/types.h"

struct ScriptTimer {
    s32 count;
    s32 period;
    s32 remaining;
    s32 trigger;
    s32 action;
};

extern struct ScriptTimer *D_8009F770;

/**
 *  @brief Sets a script timer count for the supported kind.
 *
 *  Uses the timer table to apply a count update when kind is 0x7E.
 *  Other kinds leave the table unchanged.
 *  @param index Index of the timer entry.
 *  @param kind Selector for the requested update.
 *  @param value New count value.
 *  @par Globals
 *  D_8009F770 points to the timer table.
 *  @par Control flow
 *  Checks kind once; there is no index bounds check.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May write the selected timer's count.
 *  @par Callers
 *  Confirmed: func_8004DC64.
 */
void func_8004953C(s32 index, s32 kind, s32 value)
{
    struct ScriptTimer *timers;

    timers = D_8009F770;
    if (kind == 0x7E) {
        timers[index].count = value;
    }
}
