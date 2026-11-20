// Categories: Graphics > Command Lists
void gAddPrim(unsigned int *a0, unsigned int *a1) __asm__("func_8009235C");

/**
 *  @brief Inserts a word into a 24-bit linked chain.
 *
 *  Copies the first word's low 24 bits into the second word, then stores
 *  the second word's address in the first word's low 24 bits.
 *  Both words retain their original high bytes.
 *  @param a0 Pointer to the existing chain word.
 *  @param a1 Pointer to the word to insert.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Performs two sequential masked updates without branching.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies both pointed-to words; the address is truncated to 24 bits.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gAddPrim(unsigned int *a0, unsigned int *a1)
{
    register unsigned int low asm("$6") = 0x00FFFFFFU;
    register unsigned int high asm("$7") = 0xFF000000U;
    asm("" : "+r" (low), "+r" (high));
    *a1 = (*a1 & high) | (*a0 & low);
    *a0 = (*a0 & high) | ((unsigned int)a1 & low);
}
