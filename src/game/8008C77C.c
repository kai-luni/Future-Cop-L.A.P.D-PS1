// Categories: Input > Controller Communication
#include "game/types.h"

struct Obj {
    u8 pad0[0x30];
    u8 *out;
    u8 pad34[8];
    u8 *cmd;
    u8 pad40[6];
    u8 state;
    u8 pad47[2];
    u8 flag;
    u8 count;
    u8 pad4b;
    s32 timer;
    u8 pad50[0x98];
    u8 active;
};
extern void (*D_8009C72C)(struct Obj *);

/**
 *  @brief Advance a communication timer and handle state-dependent completion.
 *
 *  Increments the timer, counts state-dependent delays, and may dispatch a
 *  callback before resetting output for commands other than 0xF3.
 *  @param self Communication object containing state, buffers, and counters.
 *  @par Globals
 *  D_8009C72C is the callback invoked when the delayed state has a nonzero flag.
 *  @par Control flow
 *  State 1 returns after counting to two or setting flag 2 and state 255;
 *  other nonzero states return while counting to four, then may call back.
 *  @par Calls
 *  D_8009C72C receives self when the callback condition is met.
 *  @par Side effects
 *  Updates timer and possibly count, flag, state, output bytes, and active.
 *  @par Callers
 *  Confirmed: func_8008BF78.
 */
void func_8008C77C(struct Obj *self)
{
    self->timer++;
    if (self->state != 0) {
        if (self->state == 1) {
            if (self->count < 2) {
                self->count++;
            } else {
                self->flag = 2;
                self->state = 255;
            }
            return;
        }
        if (self->count < 4) {
            self->count++;
            return;
        }
        if (self->flag != 0) {
            D_8009C72C(self);
        }
    }
    if (*self->cmd != 0xf3) {
        self->out[0] = 255;
        self->out[1] = 0;
        self->active = 0;
    }
}
