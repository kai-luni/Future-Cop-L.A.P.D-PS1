// Categories: Gameplay > Player | Platform > Audio
extern int gHandlePlayerEntityMsg(int mode) __asm__("func_8005908C");

/**
 * @brief Forwards mode 5 to the sound/volume handler.
 *
 * Only the "5" sub-command is passed on to func_8005908C; every other mode
 * returns 0, so the menu dispatcher can call this without checking the mode.
 * @param mode Sub-command.
 * @return The handler's result for mode 5, otherwise 0.
 * @par Globals
 * None.
 * @par Control flow
 * One equality test.
 * @par Calls
 * func_8005908C - the handler.
 * @par Side effects
 * Whatever the handler does.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 */
int func_800578B4(int mode)
{
    if (mode == 5) {
        return gHandlePlayerEntityMsg(5);
    }
    return 0;
}
