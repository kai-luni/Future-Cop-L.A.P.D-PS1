// Categories: Platform > CD-ROM
extern unsigned char gLastCommandResult __asm__("D_8009C8C4");

/**
 * @brief Returns the CD driver result byte.
 *
 * D_8009C8C4 carries the last command result the driver stored; the companion
 * of func_8008E010, which exposes the status word.
 * @return The result byte.
 * @par Globals
 * D_8009C8C4 - last result.
 * @par Control flow
 * None.
 * @par Calls
 * None.
 * @par Side effects
 * None observed.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 */
int func_8008E020(void)
{
    return gLastCommandResult;
}
