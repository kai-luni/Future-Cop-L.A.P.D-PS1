// Categories: Game Engine > Script Timers
#include "game/types.h"

struct ScriptTimer {
    s32 count;
    s32 period;
    s32 remaining;
    s32 trigger;
    s32 action;
};

extern struct ScriptTimer *gScriptTimerTicks __asm__("D_8009F770");
extern s32 gScriptTimerCount __asm__("D_8009F774");

extern s32 func_80069D68(s32 id, s32 arg1, s32 arg2, s32 arg3);
void gTickScriptTimers(s32 elapsed) __asm__("func_80049564");


/**
 *  @brief Advances active script timers and dispatches actions when triggered.
 *
 *  Each active timer loses elapsed time; an expired timer has one period
 *  added back and evaluates its trigger before dispatching its action.
 *  @param elapsed Amount subtracted from each active timer's remaining time.
 *  @par Globals
 *  D_8009F770 points to the timer array; D_8009F774 gives its length.
 *  @par Control flow
 *  Skips zero-count timers and timers with nonnegative remaining time.
 *  Each expired timer is evaluated at most once per call; a successful
 *  trigger decrements a positive count after dispatching the action.
 *  @par Calls
 *  func_80069D68 evaluates the trigger and, on success, dispatches the action.
 *  @par Side effects
 *  Updates timer remaining times and counts and may execute script actions.
 *  @par Callers
 *  Confirmed: func_80026D9C.
 */
void gTickScriptTimers(s32 elapsed)
{
    struct ScriptTimer *timer;
    s32 i;
    s32 remaining;

    timer = gScriptTimerTicks;
    for (i = 0; i < gScriptTimerCount; timer++, i++) {
        if (timer->count == 0) {
            continue;
        }
        remaining = timer->remaining - elapsed;
        timer->remaining = remaining;
        if (remaining >= 0) {
            continue;
        }
        timer->remaining = remaining + timer->period;
        if (func_80069D68(timer->trigger, 0, -1, 0) != 0) {
            func_80069D68(timer->action, 0, -1, 0);
            if (timer->count > 0) {
                timer->count--;
            }
        }
    }
}
