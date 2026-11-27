// Categories: Game Engine > State
struct State {
    char pad0[3];
    unsigned char field3;
    char pad4[3];
    unsigned char field7;
    char pad8[12];
    unsigned int field14;
};
void gInitFlatThreePointPolylinePacket(struct State *state) __asm__("func_800925B4");


/**
 *  @brief Sets three fields of a state structure to fixed values.
 *
 *  Writes 5 to field3, 0x48 to field7, and 0x55555555 to field14.
 *  The meanings of these fields are not established by this source.
 *  @param state State structure to update.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Performs three assignments without branching or looping.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies field3, field7, and field14 of state.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gInitFlatThreePointPolylinePacket(struct State *state)
{
    state->field3 = 5;
    state->field7 = 0x48;
    state->field14 = 0x55555555;
}
