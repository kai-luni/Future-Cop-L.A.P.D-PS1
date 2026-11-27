// Categories: Utilities > Bit Manipulation
void gSetPrimRawTexture(unsigned char *p, int enabled) __asm__("func_80092438");

/**
 *  @brief Sets or clears bit 0 of a byte at offset 7.
 *
 *  Updates the selected flag while preserving the other bits in the byte.
 *  The containing object's layout and the flag's meaning are not established here.
 *  @param p Pointer to the object containing the flag byte.
 *  @param enabled Nonzero to set the bit, zero to clear it.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Selects a set or clear operation based on enabled, then stores the result.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes p[7].
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gSetPrimRawTexture(unsigned char *p, int enabled)
{
    unsigned char value;

    if (enabled) {
        value = p[7] | 1;
    } else {
        value = p[7] & 0xFE;
    }
    p[7] = value;
}
