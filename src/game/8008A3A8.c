// Categories: Game Engine > Callback Tables
extern void func_8008A43C(void);
extern void func_8008A3D4(void);
extern  void (*gCallbackTable2[])(void) __asm__("D_800A3B64");

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
 */
void func_8008A3A8(void)
{
    void (**p)(void) = gCallbackTable2;
    asm volatile("" : "+r"(p));
    p[0] = func_8008A43C;
    p[1] = func_8008A3D4;
    p[-1] = 0;
    p[2] = 0;
}
