void gResetEffectPoolsStub(void) __asm__("func_80069D04");

// Categories: Game Engine > State Machine
/**
 * @brief Empty 8-byte stub called during mission teardown and global subsystem reset.
 *
 * Confirmed: compiled body is a bare return (jr $ra; nop at 0x80069D04 in asm/us/main.s) with no register or memory accesses. Category Game Engine > State Machine reflects its placement inside the fixed teardown and reset sequences of func_800249BC and func_80024D14, immediately before func_80017D20 / func_80086164.
 * Likely: compiled-out debug or subsystem cleanup hook retained as a stub in the retail executable.
 * Unknown: which subsystem originally owned this hook prior to retail stripping.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Immediate return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_800249BC (0x80024A04, subsystem teardown) and func_80024D14 (0x80024E10, global subsystem reset) / Likely: none / Unknown: none.
 */
void gResetEffectPoolsStub(void)
{
}
