// Categories: Platform > Interrupts
typedef unsigned short u16;

extern u16 gFrameClockValid __asm__("D_8009CC8A");

/**
 * @brief Returns the cached interrupt status halfword.
 *
 * D_8009CC8A holds the halfword the dispatcher latched last, so callers can
 * inspect pending lines without reading the hardware register again. The value
 * is a snapshot, not volatile-protected.
 *
 * @return The cached halfword.
 *
 * @par Globals
 * D_8009CC8A - cached status word.
 *
 * @par Control flow
 * None.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Unknown: no direct edge in edges.json.
 */
u16 func_800915D4(void)
{
    return gFrameClockValid;
}
