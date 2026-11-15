// Categories: Platform > Interrupts | Graphics > Display
struct CallbackTable {
    void (*reserved_00)(void);
    void (*reserved_04)(void);
    void (*reserved_08)(void);
    void (*reserved_0C)(void);
    void (*reserved_10)(void);
    void (*callback)(int operation, void *argument);
};

extern struct CallbackTable *D_8009DD10;

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
 */
void func_80091510(void *argument)
{
    D_8009DD10->callback(4, argument);
}
