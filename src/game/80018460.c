// Categories: Resources > Memory Management

int gReadHeapBlockSize(int *value) __asm__("func_80018460");

/**
 * @brief Reads the size word immediately preceding a heap allocation.
 *
 * @param value Pointer to the start of the allocated payload.
 * @return The signed size word from offset -4.
 *
 * @par Confirmed mechanics
 * Loads from $a0 - 4 into $v0 and returns.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80063428.
 *
 * @par Role and path
 * The map and mesh buffer setup calls this heap-header accessor.
 *
 * @par Unknown
 * The routine assumes the supplied pointer permits the header read.
 */
int gReadHeapBlockSize(int *value)
{
    return value[-1];
}
