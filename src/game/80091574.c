// Categories: Platform > Interrupts
struct CallbackTable {
    void (*reserved_00)(void);
    void (*reserved_04)(void);
    void (*reserved_08)(void);
    void (*reserved_0C)(void);
    void (*callback)(void);
};

extern struct CallbackTable *gIrqCallbackTable __asm__("D_8009DD10");

/**
 * @brief Calls the fifth entry of the interrupt callback table.
 *
 * D_8009DD10 points at a table of five function pointers; the last one (offset
 * 0x10) is invoked, which the VSync/timer setup uses for its per-frame hook.
 * @par Globals
 * D_8009DD10 - callback table pointer.
 * @par Control flow
 * One indirect call.
 * @par Calls
 * The table entry, indirectly.
 * @par Side effects
 * Whatever the callback does.
 * @par Callers
 * Confirmed: the interrupt/timer block (func_80091270 neighbourhood).
 *
 * @par Role and path
 * Performs an indirect call through slot +0x10 of D_8009DD10 with no
 * arguments in C; called by func_8001E540. Because no assignment to slot
 * +0x10 of D_8009DD10 is visible in the inspected units, the target routine
 * cannot be identified.
 */
void func_80091574(void)
{
    gIrqCallbackTable->callback();
}
