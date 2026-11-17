// Categories: Platform > State Management
#include "game/types.h"

typedef struct {
    u8 pad0[0x8F4];
    s32 state;
    s32 mode;
    s32 handle;
    s32 result;
    s32 flag;
    u8 pad908[0x9C];
    s32 active;
    s32 enabled;
    u8 pad9AC[0x1204];
    s32 reset;
} State;

extern State *gOperationState __asm__("D_8009AB08");
extern void func_8008D640(s32);
extern void func_8008D690(s32);

/**
 *  @brief Transitions the shared state and dispatches its handle to a helper.
 *
 *  The routine clears reset and enables the state before selecting a transition.
 *  State 1 has a special mode-5 transition; other cases reset or set fields.
 *  @return Always 0.
 *  @par Globals
 *  D_8009AB08 points to the state whose fields and handle are read or modified.
 *  @par Control flow
 *  States 2, 3, and 6 reset fields and set result to -3; state 1 sets result
 *  to -1 and falls through to state 4 handling. The default sets result to -2
 *  and state to 4.
 *  @par Calls
 *  func_8008D640 receives the handle for states 1, 2, 3, 4, and 6;
 *  func_8008D690 receives it for the default transition.
 *  @par Side effects
 *  Modifies shared state fields and invokes one of the handle helpers.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_8001FD28(void)
{
    register State *p asm("$2");
    State *q;
    gOperationState->reset = 0;
    gOperationState->enabled = 1;
    switch (gOperationState->state) {
    case 2:
    case 3:
    case 6:
        p = gOperationState;
        p->result = -3;
        p->mode = 0;
        p->state = 0;
        p->flag = 0;
        p->active = 1;
        asm volatile("" : : : "memory");
        goto finish;
    case 1:
        q = gOperationState;
        q->result = -1;
        asm volatile("" : : : "memory");
        if (q->mode == 5) {
            q->flag = 1;
            q->mode = 6;
            asm volatile("" : : : "memory");
        } else {
            q->mode = 0;
        }
    case 4:
        p = gOperationState;
        p->state = 0;
    finish:
        func_8008D640(p->handle);
        break;
    default:
        p = gOperationState;
        p->result = -2;
        p->mode = 0;
        p->state = 4;
        p->flag = 0;
        asm volatile("" : : : "memory");
        func_8008D690(p->handle);
        break;
    }
    return 0;
}
