int gIsQueuedCommandKind(unsigned int value) __asm__("func_8004DA58");

// Categories: Utilities > Range Checks
/**
 *  @brief Tests whether a value belongs to either of two ranges.
 *
 *  Accepts values from 60 through 69 or from 100 through 101.
 *  The unsigned subtraction checks also reject values below each range.
 *  @param value Unsigned value to test.
 *  @return 1 if the value is in either range, otherwise 0.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Checks the first range, then the second if necessary.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_8004DA84.
 */
int gIsQueuedCommandKind(unsigned int value)
{
    register unsigned int test __asm__("$2");

    test = value - 0x3C;
    if (test < 10) {
        goto matched;
    }
    test = value - 0x64;
    if (test < 2) {
        goto matched;
    }
    return 0;

matched:
    return 1;
}
