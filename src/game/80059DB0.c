// Categories: Game Engine > World Reset
/**
 * @brief Empty reset stub invoked early in the global subsystem reset sequence.
 *
 * No-op function (jr $ra; nop) declared void in 80059DB0.c; func_80024D14 passes arg0 in $a0 which is ignored.
 *
 * @param void No parameters declared.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Immediate return; no instructions besides jr $ra and nop.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_80024D14 (0x80024D58, mission reset sequence) / Likely: none / Unknown: none.
 */
void func_80059DB0(void)
{
}
