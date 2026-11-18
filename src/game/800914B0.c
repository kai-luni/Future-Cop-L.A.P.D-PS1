// Categories: Platform > Interrupts
struct CallbackTable {
    void (*reserved_00)(void);
    void (*reserved_04)(void);
    void (*callback)(void);
};

extern struct CallbackTable *gIrqCallbackTable __asm__("D_8009DD10");

/**
 * @brief Dispatches the interrupt-registration callback at offset +0x08 of D_8009DD10.
 *
 * Loads D_8009DD10->callback (slot +0x08 of CallbackTable, sibling to +0x04 in func_800914E0 and +0x0C in func_80091480) and calls it via jalr, leaving $a0 (IRQ index) and $a1 (handler pointer) intact from the caller.
 *
 * @param void No C parameters ($a0 and $a1 pass through in registers to the target callback).
 *
 * @par Globals
 * D_8009DD10 - pointer to CallbackTable; reads callback pointer at +0x08.
 *
 * @par Control flow
 * Straight-line load of D_8009DD10->callback and indirect jalr call; no branches or loops.
 *
 * @par Calls
 * Indirect call to D_8009DD10->callback (+0x08).
 *
 * @par Side effects
 * Invokes the registered interrupt-callback installer with caller's $a0/$a1 arguments.
 *
 * @par Callers
 * Confirmed: func_8001C808 (0x8001CBE8), func_8001D130 (0x8001D190), func_8008F89C (0x8008F8D0), func_8008F8E8 (0x8008F950), func_80091BA0 (0x80091BD8), func_80091CC0 (0x80091CEC), gSpuSetIrqCallback (0x80098C44).
 */
void func_800914B0(void)
{
    gIrqCallbackTable->callback();
}
