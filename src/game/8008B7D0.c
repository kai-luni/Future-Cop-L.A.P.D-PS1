// Categories: Platform > Dispatch
typedef unsigned int u32;

extern u32 gPadDataOffset __asm__("D_8009C740");
extern u32 D_8009C7A8;
extern int func_8008A9A8(u32 arg, int argvalue);

/**
 *  @brief Dispatches an argument and records the returned value.
 *
 *  Calls the function stored in a global dispatch slot, saves its result,
 *  and then passes the same argument and -2 to a second function.
 *  @param arg Value passed to both dispatched operations.
 *  @par Globals
 *  D_8009C740 supplies the function address; D_8009C7A8 receives its result.
 *  @par Control flow
 *  Calls the stored function first, then calls func_8008A9A8.
 *  @par Calls
 *  The D_8009C740 function produces the saved result.
 *  func_8008A9A8 receives arg and -2; its return value is ignored.
 *  @par Side effects
 *  Updates D_8009C7A8; called functions may have additional effects.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8008B7D0(u32 arg)
{
    u32 r;
    r = ((u32 (*)(u32))gPadDataOffset)(arg);
    D_8009C7A8 = r;
    func_8008A9A8(arg, -2);
}
