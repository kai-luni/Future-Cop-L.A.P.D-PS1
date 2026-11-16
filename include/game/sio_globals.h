#ifndef GAME_SIO_GLOBALS_H
#define GAME_SIO_GLOBALS_H

/*
 * Serial port 0 (controller / memory card) register pointers.
 *
 * 0x1F801040 JOY_DATA  transmit/receive data
 * 0x1F801044 JOY_STAT  status
 * 0x1F80104A JOY_CTRL  control
 * 0x1F80104E JOY_BAUD  baud rate
 *
 * The pointer addresses the data register; the units reach the neighbouring
 * registers through a struct or a byte offset. It keeps its retail assembler
 * symbol through an asm label, so the machine code is unchanged.
 *
 * Declare it where it is used, for example:
 *
 *   extern volatile u8 *gSio0RegBasePtr __asm__(SIO0_REGS_ASM);
 */

#define SIO0_REGS_ASM "D_8009C78C"
#define SIO0_RX_DATA_ASM "D_8009C7A4"
#define SIO0_CTRL_ASM "D_8009C7C0"
#define SIO0_CTRL_CLEAR_ASM "D_8009C804"

#endif /* GAME_SIO_GLOBALS_H */
