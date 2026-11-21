// Categories: Game Engine > State
struct State {
    unsigned char pad0[3];
    unsigned char active;
    unsigned char pad4[3];
    unsigned char current;
    unsigned char pad8[16];
    unsigned int value;
};
void gSetLineF4(struct State *state) __asm__("func_800925F4");


/**
 *  @brief Sets a state to current value 0x4C and active value 6.
 *
 *  Also writes 0x55555555 to the state's value field.
 *  The meaning of these fixed values is not established by this source.
 *  @param state State structure to update.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Performs three assignments without branching or looping.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the active, current, and value fields of state.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gSetLineF4(struct State *state)
{
    state->active = 6;
    state->current = 0x4C;
    state->value = 0x55555555;
}
