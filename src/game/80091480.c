// Categories: Platform > Interrupts | Graphics > Display
struct CallbackTable {
    void (*reserved_00)(void);
    void (*reserved_04)(void);
    void (*reserved_08)(void);
    void (*callback)(void);
};

extern struct CallbackTable *D_8009DD10;

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
 */
void func_80091480(void)
{
    D_8009DD10->callback();
}
