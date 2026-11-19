void gZeroWords(int *destination, unsigned int count) __asm__("func_80091B1C");

// Categories: Utilities > Memory
/**
 *  @brief Clears a sequence of integer elements.
 *
 *  Writes zero to each of count consecutive integers starting at destination.
 *  The caller must provide enough writable storage.
 *  @param destination First integer to clear.
 *  @param count Number of integers to clear.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Loops until count elements have been written; zero count writes nothing.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the specified destination elements with zero.
 *  @par Callers
 *  Confirmed: func_80091614.
 *
 * @par Role and path
 * Zeroes count consecutive 32-bit integers starting at destination in a
 * simple while loop. Called by func_80091614 to clear the 0x41A-word
 * interrupt subsystem state block D_8009CC88.
 */
void gZeroWords(int *destination, unsigned int count)
{
    while (count-- != 0) {
        *destination++ = 0;
    }
}
