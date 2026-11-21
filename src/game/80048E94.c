int gGetDefaultFontBankIndex(void) __asm__("func_80048E94");

// Categories: Utilities > Constants
/**
 * @brief Returns the default font/character bank index.
 *
 * @return 0, always. The body is a single "return 0" and the retail build
 *         keeps it as a real function (no inlining), so it is the place where
 *         the front-end's font choice is hard-wired.
 *
 * @par Callers
 * func_800287F4 passes the result straight into func_80048E28().
 */
int gGetDefaultFontBankIndex(void)
{
    return 0;
}
