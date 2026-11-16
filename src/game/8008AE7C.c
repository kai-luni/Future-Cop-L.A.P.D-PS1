// Categories: Game Engine > State
/**
 *  @brief Writes three values into fields of a state buffer.
 *
 *  The byte fields at offsets 0x36 and 0x35 receive a and c, while the
 *  integer field at offset 0x2c receives b.
 *  @param state Base address of the state buffer.
 *  @param a Value converted to an unsigned byte for offset 0x36.
 *  @param b Integer value stored at offset 0x2c.
 *  @param c Value converted to an unsigned byte for offset 0x35.
 *  @par Globals
 *  None referenced.
 *  @par Control flow
 *  Performs three unconditional stores in source order.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the three fields in the supplied buffer.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8008AE7C(unsigned char *state, int a, int b, int c)
{
    state[0x36] = a;
    *(int *)(state + 0x2c) = b;
    state[0x35] = c;
}
