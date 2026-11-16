// Categories: Graphics > Command Lists
/**
 *  @brief Replaces a chain word's low 24 bits while saving its old link.
 *
 *  Copies the low 24 bits of the first word into the destination word,
 *  then replaces them in the first word with the low 24 bits of b.
 *  Each word retains its high byte.
 *  @param a Pointer to the chain word to update.
 *  @param b Replacement value for the low 24 bits.
 *  @param c Pointer to the word receiving the previous low 24 bits.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Performs two sequential masked updates without branching.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the words pointed to by a and c.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80092398(unsigned int *a, unsigned int b, unsigned int *c)
{
    register unsigned int low __asm__("$7") = 0x00FFFFFFU;
    register unsigned int high __asm__("$8") = 0xFF000000U;
    __asm__("" : "+r"(low), "+r"(high));
    *c = (*c & high) | (*a & low);
    *a = (*a & high) | (b & low);
}
