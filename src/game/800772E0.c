// Categories: Platform > Interrupts | Platform > BIOS
extern unsigned int gExceptionVectorBackup[] __asm__("D_80076A0C");
extern void func_8008C9C0(void);
extern void func_8008C9E0(void);

/**
 * @brief Restores the eight words of the general exception vector from
 *        D_80076A0C with interrupts disabled.
 *
 * The exact counterpart of func_80077340, which copies the vector at 0x80 (the
 * cached alias of 0x80000080) into D_80076A0C and then overwrites its first four
 * words with a "lui $k0 / ori $k0 / jr $k0 / nop" trampoline to
 * func_80076878+0x4C. That offset is the game's own exception handler: it saves
 * $at, $v0 and the COP0 cause into D_800A6E38 and dispatches on the exception
 * code. This function undoes the patch.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Globals
 * D_80076A0C - 8-word (0x20-byte) save area for the vector, written by
 * func_80077340.
 *
 * @par Control flow
 * Disable interrupts, copy eight words in a counted loop, enable interrupts.
 *
 * @par Calls
 * func_8008C9C0 - clears COP0 status bits 0x402 (interrupt enable and
 * current-enable).
 * func_8008C9E0 - sets 0x401 again; the previous state is not restored, so the
 * pair is not nesting safe.
 *
 * @par Side effects
 * Overwrites 32 bytes at 0x80 and toggles the COP0 status register.
 *
 * @par Callers
 * Confirmed: none - no unit in the call graph reaches it.
 */
void func_800772E0(void)
{
    unsigned int *src = gExceptionVectorBackup;
    unsigned int *dst = (unsigned int *)0x80;
    int i;

    func_8008C9C0();
    for (i = 0; i < 8; i++) {
        *dst++ = *src++;
    }
    func_8008C9E0();
}
