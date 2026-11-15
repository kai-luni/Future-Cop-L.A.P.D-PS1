// Categories: Graphics > GPU > Drawing Area | Platform > GPU
extern int D_8009DD90;

/**
 * @brief Returns the GPU library's current drawing origin.
 *
 * @return The word at D_8009DD90 (the library state's origin/drawing-area field).
 *
 * @par Confirmed
 * Read-only accessor, no library call, no side effect.
 *
 * @par Callers
 * func_80092E68 and func_800934BC (the drawing-area setter compares it before
 * asking for a new area).
 */
int func_80091F74(void)
{
    return D_8009DD90;
}
