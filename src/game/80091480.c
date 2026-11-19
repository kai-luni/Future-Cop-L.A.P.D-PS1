// Categories: Platform > Interrupts | Graphics > Display
struct CallbackTable {
    void (*reserved_00)(void);
    void (*reserved_04)(void);
    void (*reserved_08)(void);
    void (*callback)(void);
};

extern struct CallbackTable *gIrqCallbackTable __asm__("D_8009DD10");
void gResetIrqDispatch(void) __asm__("func_80091480");


/**
 * @brief Dispatches the registered display/VBlank callback.
 *
 * @return Nothing; the callback's own return value is discarded.
 *
 * @par Confirmed
 * Calls D_8009DD10->callback, the fourth function pointer of the state block
 * (slots at +0, +4, +8 are unused/reserved).
 *
 * @par Likely role
 * The hook the library calls from its interrupt handler; the callback slot is
 * what the game's frame code installs.
 *
 * @par Role and path
 * Invokes the fourth function pointer (offset +0x0C) in the interrupt
 * callback table D_8009DD10 with no arguments. Called by func_8008F89C and
 * func_8008F8E8 during CD-ROM subsystem initialization and callback reset.
 */
void gResetIrqDispatch(void)
{
    gIrqCallbackTable->callback();
}
