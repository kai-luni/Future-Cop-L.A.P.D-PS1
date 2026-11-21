void gZeroIntArray(int *destination, unsigned int count) __asm__("func_80091F38");

// Categories: Utilities > Memory
/**
 *  @brief Clears a sequence of integer elements.
 *
 *  Writes zero to each element in the caller-provided destination range,
 *  providing a simple count-based memory initialization operation.
 *  @param destination First integer element to clear.
 *  @param count Number of integer elements to clear.
 *  @par Globals
 *  No named globals are referenced.
 *  @par Control flow
 *  Loops once per element; a zero count performs no writes.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites count consecutive integers at destination with zero.
 *  @par Callers
 *  Confirmed: func_80091CC0.
 */
void gZeroIntArray(int *destination, unsigned int count)
{
    while (count-- != 0) {
        *destination++ = 0;
    }
}
