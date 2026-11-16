// Categories: Platform > CD-ROM
extern unsigned char D_8009C8B4;

/**
 * @brief Returns the CD driver status byte.
 *
 * D_8009C8B4 is the status word the driver maintains; bit 0x10 tells callers
 * that a synchronous command is in flight, the low bits carry the last result.
 *
 * @return The status byte.
 * @par Globals
 * D_8009C8B4 - driver status.
 * @par Control flow
 * None.
 * @par Calls
 * None.
 * @par Side effects
 * None observed.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 */
int func_8008E010(void)
{
    return D_8009C8B4;
}
