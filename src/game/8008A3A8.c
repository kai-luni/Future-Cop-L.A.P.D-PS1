// Categories: Game Engine > Callback Tables
extern void gPollPadPorts(void) __asm__("func_8008A43C");
extern void gPadVsyncDispatchHook(void) __asm__("func_8008A3D4");
extern  void (*gCallbackTable2[])(void) __asm__("D_800A3B64");
void gInitPadCallbackTable(void) __asm__("func_8008A3A8");


/**
 *  @brief Initializes two callback entries and their surrounding null entries.
 *
 *  Populates the callback table at D_800A3B64 with two function pointers.
 *  The entries immediately before and after those pointers are cleared.
 *  @par Globals
 *  D_800A3B64 is the base of the entries being initialized.
 *  @par Control flow
 *  Writes four entries in sequence without branching or dispatching.
 *  @par Calls
 *  None; func_8008A43C and func_8008A3D4 are stored, not called.
 *  @par Side effects
 *  Changes the table entries at indices -1 through 2.
 *  @par Callers
 *  Confirmed: func_8008BDB0.
 *
 * @par Role and path
 * Stores func_8008A43C and func_8008A3D4 into gCallbackTable2[0..1] and
 * clears gCallbackTable2[-1] and gCallbackTable2[2]. Called by func_8008BDB0
 * during pad driver initialization.
 */
void gInitPadCallbackTable(void)
{
    void (**p)(void) = gCallbackTable2;
    asm volatile("" : "+r"(p));
    p[0] = gPollPadPorts;
    p[1] = gPadVsyncDispatchHook;
    p[-1] = 0;
    p[2] = 0;
}
