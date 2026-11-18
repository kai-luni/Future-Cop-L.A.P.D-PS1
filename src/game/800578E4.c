// Categories: Gameplay > Player | Game Engine > Message Dispatch
extern int gEntityBaseHandler(int mode) __asm__("func_8002AD58");

/**
 * @brief Forwards entity message 5 to the base entity handler.
 *
 * The menu/entity glue turns a sub-command into message 5 and hands it to
 * func_8002AD58; other modes return 0.
 * @param mode Sub-command.
 * @return The handler's result for mode 5, otherwise 0.
 * @par Globals
 * None.
 * @par Control flow
 * One equality test.
 * @par Calls
 * func_8002AD58 - base entity handler.
 * @par Side effects
 * Whatever the handler does.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 */
int func_800578E4(int mode)
{
    if (mode == 5) {
        return gEntityBaseHandler(5);
    }
    return 0;
}
