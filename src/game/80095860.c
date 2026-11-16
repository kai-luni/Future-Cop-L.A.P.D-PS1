// Categories: BIOS > System Calls | Platform > GPU
/* Handwritten function */
/**
 * @brief BIOS trampoline A0:49 (GPU_cw): submits a 32-bit GP0 command word to the PS1 GPU.
 *
 * Handwritten MIPS stub at 0x80095860 executing `addiu $t2, $zero, 0xA0; jr $t2; addiu $t1, $zero, 0x49` (padded with one nop), dispatching into the PS1 BIOS A0 vector table for GPU_cw(cmd). Category BIOS > System Calls | Platform > GPU reflects the BIOS A0:49 vector and GPU command-word dispatch.
 *
 * @param cmd (implicit $a0) 32-bit GP0 command word (passed as (u32)D_8009DDE8 & 0xFFFFFF from func_80092910).
 * @return (implicit $v0) BIOS A0:49 return value.
 *
 * @par Globals
 * None directly (BIOS vector 0xA0 function 0x49).
 *
 * @par Control flow
 * Straight-line BIOS tail jump setting $t2 = 0xA0 and $t1 = 0x49 in the delay slot.
 *
 * @par Calls
 * None (tail-jumps to PS1 BIOS A0 vector 0x49).
 *
 * @par Side effects
 * Issues a GP0 command word to GPU hardware via BIOS A0:49.
 *
 * @par Callers
 * Confirmed: func_80092910 (0x800929A8 in asm/us/main.s) / Likely: none / Unknown: none.
 */
__asm__(
    ".set noreorder\n"
    ".globl bios_A0_49_GPU_cw\n"
    ".ent bios_A0_49_GPU_cw\n"
    "bios_A0_49_GPU_cw:\n"
    ".word 0x240A00A0\n"
    ".word 0x01400008\n"
    ".word 0x24090049\n"
    ".word 0x00000000\n"
    ".end bios_A0_49_GPU_cw\n"
    ".set reorder\n"
);
