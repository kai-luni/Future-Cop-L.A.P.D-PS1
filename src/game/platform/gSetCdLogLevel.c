// Categories: Platform > CD-ROM | Utilities > Debug
extern int gCdLogLevel __asm__("D_8009C8B0");
int gSetCdLogLevel(int value) __asm__("func_8008E0D8");


/**
 * @brief Sets the CD-ROM driver debug verbosity D_8009C8B0 and returns the previous level.
 *
 * Reads the current verbosity word from D_8009C8B0, stores the new value via base[-0xDD4] relative to 0x800A0000 (address 0x8009C8B0), and returns the prior setting; matches the Sony libcd CdSetDebug pattern.
 *
 * @param value New debug verbosity level written to D_8009C8B0 (0 disables logging).
 * @return Previous value of D_8009C8B0 before the update.
 *
 * @par Globals
 * D_8009C8B0 - CD-ROM driver debug verbosity flag gating diagnostic prints in func_8008E7A0 and func_8008F244.
 *
 * @par Control flow
 * Straight-line load of old value, store of new value, and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Overwrites D_8009C8B0 with value.
 *
 * @par Callers
 * Confirmed: func_8001C808 (0x8001C830, 0x8001CC38, suppresses and restores CD logging) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Reads the previous debug verbosity value from gCdLogLevel (D_8009C8B0),
 * writes the new value via 0x800A0000[-0xDD4] (0x8009C8B0), and returns the
 * old value. Called by func_8001C808 to suppress and restore CD diagnostic
 * logging in func_8008E7A0 and func_8008F244.
 */
int gSetCdLogLevel(int value)
{
    register int *base __asm__("$1");
    int previous = gCdLogLevel;
    __asm__ volatile("" : "+r"(previous) : : "$1", "memory");
    base = (int *)0x800A0000;
    __asm__("" : "+r"(base));
    base[-0xDD4] = value;
    return previous;
}
