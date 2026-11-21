unsigned int gGetNextPrim(unsigned int *value) __asm__("func_80092324");

// Categories: Utilities > Bit Manipulation
/**
 *  @brief Returns the low 24 bits of a word with bit 31 set.
 *
 *  Reads the pointed-to word and constructs a new value, clearing
 *  bits 24 through 30 and setting bit 31.
 *  @param value Pointer to the word to read.
 *  @return The transformed word.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Applies a mask and bitwise OR without branching or looping.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None; the pointed-to word is not modified.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
unsigned int gGetNextPrim(unsigned int *value)
{
    return (*value & 0x00FFFFFFU) | 0x80000000U;
}
