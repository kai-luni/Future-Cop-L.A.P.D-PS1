// Categories: Game Engine > State Access
extern unsigned char gLastResultByte __asm__("D_8009C8C5");
int gGetLastCdCommand(void) __asm__("func_8008E030");


/**
 *  @brief Returns the current value of a global byte.
 *
 *  This accessor reads D_8009C8C5 and promotes its unsigned value
 *  to int. The source does not identify the byte's purpose.
 *  @return The value of D_8009C8C5, in the range 0 to 255.
 *  @par Globals
 *  D_8009C8C5 is read as an unsigned byte.
 *  @par Control flow
 *  Returns directly without branching or looping.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None; the global is not modified.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Reads and returns gLastResultByte (D_8009C8C5), which records the last
 * CD-ROM command byte written in func_8008F244. Performs no function calls
 * and has no direct callers in src/.
 */
int gGetLastCdCommand(void)
{
    return gLastResultByte;
}
