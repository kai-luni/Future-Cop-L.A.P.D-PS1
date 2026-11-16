// Categories: Game Engine > Callback Setup
extern void func_8008C444(void);
extern void func_8008C858(void);
extern void func_8008C518(void);
extern void (*D_8009C740)(void);
extern void (*D_8009C744)(void);

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
 */
void func_8008C410(void)
{
    D_8009C740 = func_8008C444;
    D_8009C744 = func_8008C858;
    {
        register char *base __asm__("$1");
        register void (*value)(void) __asm__("$2");
        __asm__("" ::: "memory");
        value = func_8008C518;
        __asm__("" : "+r" (value) : : "memory");
        base = (char *)0x800a0000;
        __asm__("" : "+r" (base));
        *(void (**)(void))(base - 14520) = value;
    }
}
