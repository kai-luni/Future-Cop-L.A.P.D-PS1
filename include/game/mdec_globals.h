#ifndef GAME_MDEC_GLOBALS_H
#define GAME_MDEC_GLOBALS_H

/*
 * Macroblock decoder register pointers.
 *
 * 0x1F801820 MDEC0 command/parameter port
 * 0x1F801824 MDEC1 control/status word:
 *            write 0x80000000 resets the unit,
 *            write 0x60000000 enables data input and output,
 *            bit 0x20000000 reads as "input FIFO full".
 *
 * The pointers keep their retail assembler symbols through asm labels, so the
 * machine code is unchanged. Declare them where they are used:
 *
 *   extern volatile u32 *gMdecControlStatusPtr __asm__(MDEC_CONTROL_ASM);
 */

#define MDEC_COMMAND_ASM "D_8009E9F0"
#define MDEC_CONTROL_ASM "D_8009E9F4"

#endif /* GAME_MDEC_GLOBALS_H */
