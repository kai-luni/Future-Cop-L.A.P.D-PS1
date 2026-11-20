// Categories: Utilities > Bit Manipulation
void gSetPrimNextLink(unsigned int *p, unsigned int value) __asm__("func_800923D4");

/**
 *  @brief Replaces the low 24 bits of a word while preserving its high byte.
 *
 *  Masks value to 24 bits and combines it with the existing high byte at p.
 *  The upper eight bits of value do not affect the result.
 *  @param p Pointer to the writable word to update.
 *  @param value Source of the replacement low 24 bits.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Performs one unconditional masked assignment.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the word pointed to by p.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gSetPrimNextLink(unsigned int *p, unsigned int value)
{
    *p = (*p & 0xff000000U) | (value & 0x00ffffffU);
}
