// Categories: Utilities > Bit Manipulation
int gIsPrimListEnd(unsigned int *value) __asm__("func_80092340");

/**
 *  @brief Tests whether the low 24 bits of a word are all set.
 *
 *  Reads the pointed-to word and compares its masked low 24 bits
 *  with 0x00FFFFFF, ignoring the upper eight bits.
 *  @param value Pointer to the word to test.
 *  @return 1 if all low 24 bits are set, otherwise 0.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Masks and compares the word without looping.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None; the pointed-to word is not modified.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
int gIsPrimListEnd(unsigned int *value)
{
    return (*value & 0x00FFFFFFU) == 0x00FFFFFFU;
}
