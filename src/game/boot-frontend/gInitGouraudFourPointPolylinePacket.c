// Categories: Game Engine > State
struct State {
    unsigned char padding0[3];
    unsigned char field3;
    unsigned char padding4[3];
    unsigned char field7;
    unsigned char padding8[28];
    unsigned int field24;
};
void gInitGouraudFourPointPolylinePacket(struct State *state) __asm__("func_80092614");


/**
 *  @brief Initializes three fields of a state structure to fixed values.
 *
 *  Writes 9 to field3, 0x5c to field7, and 0x55555555 to field24.
 *  Other structure fields are left unchanged.
 *  @param state State structure to update; must be valid.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Performs three unconditional assignments.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the three specified fields of state.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gInitGouraudFourPointPolylinePacket(struct State *state)
{
    state->field3 = 9;
    state->field7 = 0x5c;
    state->field24 = 0x55555555;
}
