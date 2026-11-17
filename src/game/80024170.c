// Categories: Game Engine > Mode Flag
extern int gPlayerCountRecord __asm__("D_800A0A88");

/**
 * @brief Reports whether the game is in the single-player/one-player mode.
 *
 * D_800A0A88 holds the player count record; only the value 1 counts as the
 * one-player case. Used by the front-end to hide the split-screen panels.
 *
 * @return 1 when the mode word equals 1, else 0.
 * @par Globals
 * D_800A0A88 - player mode word.
 * @par Control flow
 * None.
 * @par Calls
 * None.
 * @par Side effects
 * None observed.
 * @par Callers
 * Unknown: no direct edge; the front-end queries it through its mode dispatch.
 */
int func_80024170(void)
{
    return gPlayerCountRecord == 1;
}
