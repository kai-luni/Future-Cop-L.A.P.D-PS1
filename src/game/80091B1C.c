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
 */
void func_80091B1C(int *destination, unsigned int count)
{
    while (count-- != 0) {
        *destination++ = 0;
    }
}
