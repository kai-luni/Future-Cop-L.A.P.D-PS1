// Categories: Platform > Asynchronous Operations
typedef struct {
    char pad0[0x8f4];
    volatile unsigned int state;
    volatile int field8f8;
    volatile int field8fc;
    volatile int field900;
    volatile int field904;
    char pad908[0xc];
    char field914[0x84];
    volatile int field998;
    char pad99c[4];
    volatile int counter;
    volatile int done;
    char pad9a8[0x1208];
    volatile int field1bb0;
} State;

extern State *volatile gOperationState __asm__("D_8009AB08");
extern void func_8008D640(int);
extern void func_8008D670(int, int, void *);
extern void func_8008D680(int, int, void *);
int gMcRetryTimeoutB(void) __asm__("func_8001F968");


/**
 *  @brief Advances or terminates an operation according to its state and counter.
 *
 *  Clears a state field, dispatches an operation using the stored handle and
 *  buffer, or marks the operation done and closes the handle.
 *  @return Always zero.
 *  @par Globals
 *  D_8009AB08 points to the shared operation state.
 *  @par Control flow
 *  States 2, 3, and 6 continue only while the counter is positive; other states
 *  take the default completion path.
 *  @par Calls
 *  func_8008D680 handles state 2; func_8008D670 handles states 3 and 6;
 *  func_8008D640 closes the handle on completion.
 *  @par Side effects
 *  Updates the counter or completion fields; timed completion also sets field900 to -3.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Handles retry/timeout for gOperationState (D_8009AB08): clears field1bb0,
 * then for states 2, 3, and 6 checks counter > 0, decrements counter, and
 * re-dispatches func_8008D680 (state 2) or func_8008D670 (states 3 and 6); on
 * expiration or other states it sets done to 1, clears field8f8/state, and
 * calls func_8008D640(field8fc).
 */
int gMcRetryTimeoutB(void)
{
    register State *p asm("$6") = gOperationState;
    p->field1bb0 = 0;
    switch (p->state) {
    case 2:
        if (p->counter > 0) {
            p->counter--;
            func_8008D680(p->field8fc, p->field998, p->field914);
        } else {
            p->done = 1;
            p->field8f8 = 0;
            p->state = 0;
            p->field904 = 0;
            p->field900 = -3;
            func_8008D640(p->field8fc);
        }
        break;
    case 3:
    case 6:
        p = gOperationState;
        if (p->counter > 0) {
            p->counter--;
            func_8008D670(p->field8fc, p->field998, p->field914);
        } else {
            p->done = 1;
            p->field8f8 = 0;
            p->state = 0;
            p->field904 = 0;
            p->field900 = -3;
            func_8008D640(p->field8fc);
        }
        break;
    default:
        {
            State *q = gOperationState;
            q->done = 1;
            q->field8f8 = 0;
            q->state = 0;
            func_8008D640(q->field8fc);
        }
        break;
    }
    return 0;
}
