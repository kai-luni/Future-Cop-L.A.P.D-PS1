// Categories: Platform > Memory Card | BIOS > System Calls
extern int D_8009C800;

/**
 * @brief Returns the BIOS card/pad initialization status word D_8009C800.
 *
 * Reads and returns D_8009C800 (set to 1 by func_8008D7DC and func_8008D86C after kernel table patching). Used by func_8008D6E0 to force the InitCARD mode argument to 0 when D_8009C800 is 0. Category Platform > Memory Card | BIOS > System Calls reflects the card init caller and BIOS status flag.
 *
 * @param void No parameters.
 * @return Value of D_8009C800 (0 if uninitialized/cleared, 1 after pad/BIOS init).
 *
 * @par Globals
 * D_8009C800 - BIOS/pad initialization status word returned verbatim.
 *
 * @par Control flow
 * Straight-line load and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8008D6E0 (0x8008D700 in asm/us/main.s) / Likely: none / Unknown: none.
 */
int func_8008D7CC(void)
{
    return D_8009C800;
}
