// Categories: Game Engine > State
struct State {
    char pad0[3];
    unsigned char a;
    char pad1[3];
    unsigned char b;
    char pad2[20];
    unsigned int c;
};

/**
 *  @brief Sets three fields of a state structure to fixed values.
 *
 *  Writes 7 to a, 0x58 to b, and 0x55555555 to c.
 *  The meanings of these fields are not established by this source.
 *  @param state State structure to update.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Performs three assignments without branching or looping.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the a, b, and c fields of state.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_800925D4(struct State *state)
{
    state->a = 7;
    state->b = 0x58;
    state->c = 0x55555555;
}
