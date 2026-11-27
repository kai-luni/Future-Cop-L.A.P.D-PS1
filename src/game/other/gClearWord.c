void gClearWord(int *value) __asm__("func_8001C470");

// Categories: Platform > CD-ROM | Utilities > Memory Management
/**
 * @brief Zeroes the 32-bit integer word pointed to by value.
 *
 * Writes 0 to *value. Both CD-ROM reader callers pass D_8009F61C to clear its initial status word before installing the sector callback.
 *
 * @param value Pointer to the 32-bit word to clear.
 *
 * @par Globals
 * None directly (callers pass D_8009F61C).
 *
 * @par Control flow
 * Single store instruction; no branches or loops.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Overwrites *value with 0.
 *
 * @par Callers
 * Confirmed: func_8001C808 (0x8001CC00), func_8001D130 (0x8001D1A8).
 *
 * @par Role and path
 * Writes 0 to the 32-bit integer pointed to by value with no direct global
 * accesses or outgoing calls. Called by func_8001C808 and func_8001D130 to
 * clear gCdCurrentLoc before installing the CD-ROM sector callback.
 */
void gClearWord(int *value)
{
    *value = 0;
}
