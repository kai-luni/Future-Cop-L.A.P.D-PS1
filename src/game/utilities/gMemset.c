// Categories: Utilities > Memory | Platform > Library
void gMemset(unsigned char *destination, int value, unsigned int count) __asm__("func_80095830");

/**
 * @brief Byte-wise fill of a memory range (memset form).
 *
 * @param destination Start address.
 * @param value Byte value written.
 * @param count Number of bytes.
 * @return Nothing.
 *
 * @par Confirmed
 * while (count-- != 0) *destination++ = value; no word-wise shortcut.
 *
 * @par Callers
 * func_80092910, func_80092BF4, func_80094F4C.
 */
void gMemset(unsigned char *destination, int value, unsigned int count)
{
    while (count-- != 0) {
        *destination++ = value;
    }
}
