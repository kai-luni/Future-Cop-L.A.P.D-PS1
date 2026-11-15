// Categories: Graphics > Text > Debug Numbers | Platform > Debug
extern void func_800770A8(int, int, int);

/**
 * @brief Prints a 32-bit value as eight hexadecimal digits.
 *
 * @param x, y Screen position of the first digit.
 * @param value The number; each nibble is drawn from bit 28 downwards.
 * @return Nothing.
 *
 * @par Confirmed
 * Loop `for (shift = 28; shift >= 0; shift -= 4)`: draws
 * `func_800770A8(x, y, (value >> shift) & 15)` and advances x by 7 pixels - the
 * fixed 8x8 digit cell of the debug font.
 *
 * @par Callers
 * func_8007757C and func_800773D8 (the debug screens).
 */
void func_80077278(int x, int y, unsigned int value)
{
    int shift;
    for (shift = 28; shift >= 0; shift -= 4) {
        func_800770A8(x, y, (value >> shift) & 15);
        x += 7;
    }
}
