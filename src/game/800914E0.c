// Categories: Platform > Interrupts | Graphics > Display
struct CallbackTable {
    void (*reserved_00)(void);
    void (*callback)(void);
};

extern struct CallbackTable *gIrqCallbackTable __asm__("D_8009DD10");

/**
 * @brief Dispatches the registered callback in slot +4 of the display
 *        callback block.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Single indirect call D_8009DD10->callback() - the second function
 * pointer of the block (slot +0 reserved). Sibling of func_80091480,
 * which dispatches slot +0x0C of the same block.
 *
 * @par Likely role
 * One of the library's interrupt/display hooks; the game installs the
 * handler into the slot.
 *
 * @par Callers
 * func_8008E5D8, func_80092AE0, func_80094A3C, func_80094CEC,
 * func_800953F0, func_800954DC, func_800955C8, func_8009570C,
 * func_80095808, func_80096108, func_8009612C, gSpuSetDmaCallback.
 */
void func_800914E0(void)
{
    gIrqCallbackTable->callback();
}
