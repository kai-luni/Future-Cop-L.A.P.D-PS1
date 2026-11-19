// Categories: Game Engine > Callback Dispatch
struct CallbackTable {
    void (*reserved_00)(void);
    void (*reserved_04)(void);
    void (*reserved_08)(void);
    void (*reserved_0c)(void);
    void (*reserved_10)(void);
    void (*callback)(void);
};

extern struct CallbackTable *gIrqCallbackTable __asm__("D_8009DD10");

/**
 *  @brief Invokes the callback at offset 0x14 in the current table.
 *
 *  Dispatches through the callback table referenced by D_8009DD10.
 *  The table pointer and callback are used without null checks.
 *  @par Globals
 *  D_8009DD10 supplies the callback table pointer.
 *  @par Control flow
 *  Performs one indirect call without branching.
 *  @par Calls
 *  The table callback is invoked with no arguments.
 *  @par Side effects
 *  Any side effects are those of the invoked callback.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Performs an unconditional indirect call through slot +0x14 of D_8009DD10
 * without setting arguments in C. No callers appear in src/, so the register
 * arguments passed through and specific operation cannot be proved.
 */
void func_80091544(void)
{
    gIrqCallbackTable->callback();
}
