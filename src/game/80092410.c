// Categories: Utilities > Bit Manipulation
/**
 *  @brief Sets or clears bit 1 of an object's byte at offset 7.
 *
 *  Updates the selected flag while preserving the other bits in the byte.
 *  The object's broader layout and the flag's meaning are not established here.
 *  @param obj Pointer to the object containing the flag byte.
 *  @param enabled Nonzero to set the bit, zero to clear it.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Selects a set or clear operation based on enabled, then stores the result.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes obj[7].
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80092410(unsigned char *obj, int enabled)
{
    unsigned char value;

    if (enabled) {
        value = obj[7] | 2;
    } else {
        value = obj[7] & 0xFD;
    }
    obj[7] = value;
}
