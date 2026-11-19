// Categories: Platform > GPU | Platform > DMA
extern void gWaitImageTransferFrame(void) __asm__("func_80094CEC");
extern void gSetDmaCallback(int, void (*)(void)) __asm__("func_800914E0");

/**
 * @brief Installs func_80094CEC as the channel-2 (GPU) DMA completion callback via func_800914E0.
 *
 * Invokes func_800914E0(2, func_80094CEC) so D_8009DD10->callback (func_80091E8C) registers func_80094CEC to drain the D_800A9450 GPU/DMA2 queue on completion. Callers func_800953F0, func_800954DC, func_800955C8, and func_8009570C also pass func_80095808 as a literal callback argument to func_800914E0.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None directly; func_800914E0 dispatches through D_8009DD10->callback.
 *
 * @par Control flow
 * Loads func_80094CEC address into $a1, passes 2 in $a0, and calls func_800914E0.
 *
 * @par Calls
 * func_800914E0 - invokes D_8009DD10->callback(2, func_80094CEC) to install the DMA2 handler.
 *
 * @par Side effects
 * Registers func_80094CEC on DMA channel 2 via D_8009DD10->callback.
 *
 * @par Callers
 * Confirmed: none direct; address passed to func_800914E0 in func_800953F0 (0x8009549C), func_800954DC (0x80095588), func_800955C8 (0x8009567C), and func_8009570C (0x800957CC) / Likely: invoked by DMA2 IRQ via D_8009DD10 / Unknown: none.
 */
void func_80095808(void)
{
    register void (*callback)(void) asm("$5");
    asm("lui %0, %%hi(func_80094CEC)\n\taddiu %0, %0, %%lo(func_80094CEC)" : "=r"(callback));
    gSetDmaCallback(2, callback);
}
