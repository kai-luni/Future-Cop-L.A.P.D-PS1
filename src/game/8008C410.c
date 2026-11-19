// Categories: Game Engine > Callback Setup
extern void gDispatchPadStep(void) __asm__("func_8008C444");
extern void gCheckPadReady(void) __asm__("func_8008C858");
extern void gHandlePadResponse(void) __asm__("func_8008C518");
extern  void (*gPadDataOffset)(void) __asm__("D_8009C740");
extern  void (*gReadyCheckHook)(void) __asm__("D_8009C744");
void gInstallPadProtoHooks(void) __asm__("func_8008C410");


/**
 *  @brief Installs three function pointers in global callback slots.
 *
 *  Assigns two named global slots and a third slot at address 0x8009C748.
 *  The installed functions are not invoked by this routine.
 *  @par Globals
 *  D_8009C740 and D_8009C744 receive the first two function pointers.
 *  The slot at 0x8009C748 receives the third pointer.
 *  @par Control flow
 *  Performs three assignments in sequence without branching.
 *  @par Calls
 *  None; func_8008C444, func_8008C858, and func_8008C518 are stored.
 *  @par Side effects
 *  Replaces the contents of three function-pointer slots.
 *  @par Callers
 *  Confirmed: func_8008BDB0.
 *
 * @par Role and path
 * Called by func_8008BDB0 to install three pad protocol callbacks:
 * func_8008C444 into gPadDataOffset (D_8009C740), func_8008C858 into
 * gReadyCheckHook (D_8009C744), and func_8008C518 into address 0x8009C748
 * (0x800A0000 - 14520). Performs no branches or calls.
 */
void gInstallPadProtoHooks(void)
{
    gPadDataOffset = gDispatchPadStep;
    gReadyCheckHook = gCheckPadReady;
    {
        register char *base __asm__("$1");
        register void (*value)(void) __asm__("$2");
        __asm__("" ::: "memory");
        value = gHandlePadResponse;
        __asm__("" : "+r" (value) : : "memory");
        base = (char *)0x800a0000;
        __asm__("" : "+r" (base));
        *(void (**)(void))(base - 14520) = value;
    }
}
