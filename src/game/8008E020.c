// Categories: Platform > CD-ROM
extern unsigned char gLastCommandResult __asm__("D_8009C8C4");
int gGetCdModeByte(void) __asm__("func_8008E020");


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
 *
 * @par Role and path
 * Reads and returns gLastCommandResult (D_8009C8C4), which caches the
 * parameter byte of CD command 14 (CdlSetmode) in func_8008F244. Performs no
 * function calls and has no direct callers in src/.
 */
int gGetCdModeByte(void)
{
    return gLastCommandResult;
}
