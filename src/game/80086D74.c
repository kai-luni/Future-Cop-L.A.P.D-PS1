// Categories: Game Engine > State
typedef struct State {
    char pad[0x7B20];
    int active;
    volatile int value;
    volatile int count;
} State;

extern State *D_8009F4D8;

/**
 *  @brief Advances two state counters when the state is active.
 *
 *  Checks the active field of the shared state and, when nonzero,
 *  adds 0x100 to its volatile value and increments its volatile count.
 *  @par Globals
 *  D_8009F4D8 points to the state whose fields are read and updated.
 *  @par Control flow
 *  A single active-state branch controls both updates.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May modify the state's value and count fields.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80086D74(void)
{
    State *state = D_8009F4D8;
    if (state->active != 0) {
        state->value += 0x100;
        state->count++;
    }
}
