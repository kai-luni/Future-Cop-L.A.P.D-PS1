// Categories: Game Engine > Callback Dispatch
struct CallbackTable {
    void (*reserved[6])(void);
    void (*callback)(void);
};

extern struct CallbackTable *D_8009DD10;

/**
 *  @brief Invokes the callback at offset 0x18 in the current table.
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
 */
void func_800915A4(void)
{
    D_8009DD10->callback();
}
