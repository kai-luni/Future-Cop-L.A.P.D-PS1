#ifndef GAME_GPU_GLOBALS_H
#define GAME_GPU_GLOBALS_H

/*
 * GPU port pointers.
 *
 * Retail stores the two GPU port addresses in writable pointer variables:
 *   0x8009DEC4 -> 0x1F801810  GP0 command port / GPUREAD
 *   0x8009DEC8 -> 0x1F801814  GP1 command port / GPUSTAT
 * See docs/globals/ for the full mapping and the evidence.
 *
 * Canonical C names are gGpuGp0PortPtr (GP0) and gGpuGp1PortPtr (GP1). Both
 * keep the retail assembler symbol through an asm label, so the generated
 * machine code is unchanged. No object is defined for them: the retail bytes
 * stay in asm/us/main.s.
 *
 * The pointers are declared where they are used, not here: the units need
 * different qualification (most a volatile pointee, one a volatile pointer,
 * a few a plain pointer), and one shared type changes code generation. Bind
 * each declaration to the retail symbol with the labels below, for example:
 *
 *   extern volatile u32 *gGpuGp0PortPtr __asm__(GPU_GP0_PORT_ASM);
 *   extern u32 *gGpuGp1PortPtr __asm__(GPU_GP1_PORT_ASM);
 */

#define GPU_GP0_PORT_ASM "D_8009DEC4"
#define GPU_GP1_PORT_ASM "D_8009DEC8"

#endif /* GAME_GPU_GLOBALS_H */
