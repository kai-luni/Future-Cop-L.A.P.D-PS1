// Categories: Game Engine > State
typedef struct State {
    char pad[0x7B20];
    int active;
    volatile int value;
    volatile int count;
} State;

extern State *gMovieAudioState __asm__("D_8009F4D8");
void gAdvanceMovieAudioCounters(void) __asm__("func_80086D74");


/**
 *  @brief Advances two state counters when the state is active.
 *
 *  Checks the active field of the shared state and, when nonzero,
 *  adds 0x100 to its volatile value and increments its volatile count.
 *  @par Globals
 *  gMovieAudioState points to the state whose fields are read and updated.
 *  @par Control flow
 *  A single active-state branch controls both updates.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May modify the state's value and count fields.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gAdvanceMovieAudioCounters(void)
{
    State *state = gMovieAudioState;
    if (state->active != 0) {
        state->value += 0x100;
        state->count++;
    }
}
