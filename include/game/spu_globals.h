#ifndef GAME_SPU_GLOBALS_H
#define GAME_SPU_GLOBALS_H

/*
 * Sound processing unit register pointers.
 *
 * 0x1F801C00 SPU voice 0 base: 24 voices of 16 bytes, then the global
 *             registers at +0x180 (main volume), +0x18C (reverb),
 *             +0x1A6 (transfer address), +0x1A8 (transfer FIFO),
 *             +0x1AA (control), +0x1AC (transfer control),
 *             +0x1AE (status).
 * 0x1F801014 SPU delay and reverb read address.
 *
 * The base pointer is typed per unit as a struct over that layout; it keeps
 * its retail assembler symbol through an asm label, so the machine code is
 * unchanged. Declare it where it is used, for example:
 *
 *   extern volatile struct Spu *gSpuRegBasePtr __asm__(SPU_REGS_ASM);
 */

#define SPU_REGS_ASM "D_8009EEA4"
#define SPU_REGS_ALT_ASM "D_8009CB80"
#define SPU_ACCESS_DELAY_ASM "D_8009EEB8"
#define SPU_DELAY_REGS_ASM "D_8009EEB4"

#endif /* GAME_SPU_GLOBALS_H */
