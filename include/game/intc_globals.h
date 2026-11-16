#ifndef GAME_INTC_GLOBALS_H
#define GAME_INTC_GLOBALS_H

/*
 * Interrupt controller register pointers.
 *
 * 0x1F801070 I_STAT  interrupt status / acknowledge (write 0 clears a bit)
 * 0x1F801074 I_MASK  interrupt mask
 *
 * The two registers are adjacent, so some units address both through one
 * pointer (a two-word struct or an offset of 4). Each pointer keeps its retail
 * assembler symbol through an asm label, so the machine code is unchanged.
 *
 * Declare the pointer where it is used and bind it with these labels:
 *
 *   extern volatile u32 *gInterruptStatusPtr __asm__(INT_STATUS_ASM);
 *   extern volatile u16 *gInterruptMaskPtr __asm__(INT_MASK_ASM);
 */

#define INT_STATUS_ASM "D_8009C788"
#define INT_MASK_ASM "D_8009DD18"

#endif /* GAME_INTC_GLOBALS_H */
