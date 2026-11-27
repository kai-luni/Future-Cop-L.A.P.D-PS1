// Categories: Platform > Memory Card | Resources > File Access
extern int gReadCardOperationResult(int) __asm__("func_800207B4");

int gIsCardResultUsable(int arg0) __asm__("func_80020A9C");

/**
 * @brief Converts the current card operation result to a Boolean for the
 * directory lookup path.
 *
 * @param arg0 Word forwarded to the status accessor. The accessor ignores it.
 * @return One for -2, -1, or a nonnegative checked result, otherwise zero.
 *
 * @par Confirmed mechanics
 * Reads the status once and reads it again for -3. Accepts -1 and -2, and
 * otherwise shifts the bitwise complement right by 31, testing for a
 * nonnegative result.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gFindCardFileSlot.
 *
 * @par Role and path
 * The name lookup checks this result before consulting the directory frames.
 *
 * @par Unknown
 * The routine does not establish the individual meanings of -1, -2, and -3.
 */
int gIsCardResultUsable(int arg0)
{
    register int value __asm__("$4");
    register int result __asm__("$2");

    value = gReadCardOperationResult(arg0);
    if (value == -2)
        return 1;
    if (value == -3) {
        value = gReadCardOperationResult(arg0);
        if (value == -2)
            return 1;
        result = 1;
        __asm__("" : "+r"(result));
        if (value == -1)
            return result;
        return (unsigned int)~value >> 31;
    }
    if (value == -1)
        return 1;
    return (unsigned int)~value >> 31;
}
