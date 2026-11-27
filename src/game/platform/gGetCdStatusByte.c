// Categories: Platform > CD-ROM
extern unsigned char gCdStatusByte __asm__("D_8009C8B4");
int gGetCdStatusByte(void) __asm__("func_8008E010");


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
 *
 * @par Role and path
 * Reads and returns gCdStatusByte (D_8009C8B4), the CD-ROM driver status byte
 * maintained by func_8008E7A0. Performs no function calls and has no direct
 * callers in src/.
 */
int gGetCdStatusByte(void)
{
    return gCdStatusByte;
}
