// Categories: Platform > Interrupts
typedef unsigned short u16;

extern u16 gIrqDispatchBusy __asm__("D_8009CC8A");
u16 gGetIrqLatchFlag(void) __asm__("func_800915D4");


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
 *
 * @par Role and path
 * Returns the 16-bit cached interrupt dispatch flag D_8009CC8A, which is
 * cleared at the end of func_800916EC. Polled by func_8008ECFC,
 * func_8008EF7C, and func_8008F244 to check whether interrupt dispatch is
 * active.
 */
u16 gGetIrqLatchFlag(void)
{
    return gIrqDispatchBusy;
}
