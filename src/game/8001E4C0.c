// Categories: Utilities > Memory
/**
 * @brief memset(): fills @c count bytes at @c destination with @c value.
 *
 * @param destination Start address.
 * @param value Byte value, only the low 8 bits are used.
 * @param count Number of bytes; 0 writes nothing.
 * @return The unchanged destination pointer.
 *
 * @par Note
 * The byte-wise loop is what the retail build does (no word-wide fast path),
 * so it is also the reason this helper appears in so many units.
 *
 * @par Callers
 * The whole engine: decoder set-up, movie context, image jobs, the object table
 * and every structure that has to start out zeroed.
 */
void *func_8001E4C0(void *destination, int value, unsigned int count)
{
    unsigned char *current = destination;
    void *result = destination;

    while (count != 0) {
        *current++ = value;
        count--;
    }
    return result;
}
