void gDisableCpuInterrupts(void) __asm__("func_8008C9C0");

// Categories: Platform > Interrupts | Platform > CPU
/**
 * @brief Enters a critical section: disables interrupts.
 *
 * Reads COP0 status ($12), clears the mask 0x402 (interrupt enable and the
 * matching current-enable bit) and writes it back. No value is saved, so this
 * is the "unconditional disable" variant rather than the nesting-aware
 * enter/exit pair of the Sony library.
 *
 * @return Nothing.
 *
 * @par Callers
 * func_800861E4, around the job set-up and again in the cleanup path.
 *
 * @par Role and path
 * Unconditionally disables CPU interrupts by reading COP0 status register
 * $12, masking with -0x402, and writing the result back to $12 without saving
 * the prior state. Called by func_800861E4 around job setup and cleanup.
 */
void gDisableCpuInterrupts(void) {
    __asm__ volatile (
        "mfc0 $4, $12\n\t"
        "nop\n\t"
        "addiu $1, $0, -0x402\n\t"
        "and $4, $4, $1\n\t"
        "mtc0 $4, $12\n\t"
        "nop\n\t"
    );
}
