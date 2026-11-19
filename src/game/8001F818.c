// Categories: Platform > Asynchronous Operations
typedef struct State {
    char pad0[0x8F4];
    volatile unsigned int mode;
    volatile int active;
    volatile int handle;
    volatile int code;
    volatile int other;
    char pad908[0xC];
    char data[0x84];
    volatile int value;
    int pad99c;
    volatile int timer;
    volatile int done;
    char pad9a8[0x1208];
    volatile int field1bb0;
} State;

extern State *gOperationState __asm__("D_8009AB08");
extern void func_8008D640(int);
extern void func_8008D670(int, int, void *);
extern void func_8008D680(int, int, void *);
int gMcRetryTimeoutA(void) __asm__("func_8001F818");


/**
 *  @brief Advances or terminates an operation according to its mode and timer.
 *
 *  Clears a state field, dispatches an operation using the stored handle and
 *  buffer, or marks the operation done and closes the handle.
 *  @return Always zero.
 *  @par Globals
 *  D_8009AB08 points to the shared operation state.
 *  @par Control flow
 *  Mode 2 continues only for a positive timer; modes 3 and 6 continue for any
 *  nonzero timer. Other modes take the default completion path.
 *  @par Calls
 *  func_8008D680 handles mode 2; func_8008D670 handles modes 3 and 6;
 *  func_8008D640 closes the handle on completion.
 *  @par Side effects
 *  Updates the timer or completion fields; timed completion also sets code to -3.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Handles retry/timeout for gOperationState (D_8009AB08): clears field1bb0,
 * then for mode 2 (timer > 0) or modes 3 and 6 (timer != 0) decrements timer
 * and re-dispatches func_8008D680 or func_8008D670; when the timer expires it
 * sets done to 1, code to -3, clears active/mode/other, and calls
 * func_8008D640(handle).
 */
int gMcRetryTimeoutA(void)
{
    register State *p asm("$6") = gOperationState;
    p->field1bb0 = 0;
    switch (p->mode) {
    case 2:
        if (p->timer > 0) {
            p->timer--;
            func_8008D680(p->handle, p->value, p->data);
        } else {
            p->done = 1;
            p->active = 0;
            p->mode = 0;
            p->other = 0;
            p->code = -3;
            func_8008D640(p->handle);
        }
        break;
    case 3:
    case 6:
        p = gOperationState;
        if (p->timer != 0) {
            p->timer--;
            func_8008D670(p->handle, p->value, p->data);
        } else {
            p->done = 1;
            p->active = 0;
            p->mode = 0;
            p->other = 0;
            p->code = -3;
            func_8008D640(p->handle);
        }
        break;
    default: {
        State *q = gOperationState;
        q->done = 1;
        q->active = 0;
        q->mode = 0;
        func_8008D640(q->handle);
        break;
    }
    }
    return 0;
}
