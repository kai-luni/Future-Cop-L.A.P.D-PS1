// Categories: Platform > Interrupts | Graphics > Display
struct CallbackTable {
    void (*reserved_00)(void);
    void (*reserved_04)(void);
    void (*reserved_08)(void);
    void (*reserved_0C)(void);
    void (*reserved_10)(void);
    void (*callback)(int operation, void *argument);
};

extern struct CallbackTable *gIrqCallbackTable __asm__("D_8009DD10");
void gSetVSyncCallback(void *argument) __asm__("func_80091510");


/**
 * @brief Issues callback operation 4 with an argument.
 *
 * @param argument Pointer/value forwarded to the callback.
 * @return Nothing.
 *
 * @par Confirmed
 * Calls D_8009DD10->callback(4, argument) - the fifth function pointer of the
 * block (slots at +0, +4, +8, +0xC, +0x10 are reserved, the callback sits at
 * +0x14). func_80091480 calls operation-less form of the same slot.
 *
 * @par Likely role
 * The "flush/execute queued commands" entry of the display driver; the argument
 * is a command buffer the driver walks.
 *
 * @par Role and path
 * Calls slot +0x14 of D_8009DD10 with operation code 4 and argument,
 * installing or uninstalling a per-frame VSync callback (passing 0 to
 * uninstall). Driven by func_8001F0B8, func_8001F158, func_80020530,
 * func_8002059C, func_80087750, and func_80087824.
 */
void gSetVSyncCallback(void *argument)
{
    gIrqCallbackTable->callback(4, argument);
}
