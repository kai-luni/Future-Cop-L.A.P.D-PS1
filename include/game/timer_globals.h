#ifndef GAME_TIMER_GLOBALS_H
#define GAME_TIMER_GLOBALS_H

/*
 * Root counter (timer) register pointers.
 *
 * 0x1F801110 timer 1 counter  0x1F801114 timer 1 mode
 * 0x1F801118 timer 1 target
 *
 * The counter is read twice and compared to obtain a stable value; the mode
 * register is written once at start-up. The pointers keep their retail
 * assembler symbols through asm labels, so the machine code is unchanged.
 *
 *   extern volatile u32 *gTimer1CounterPtr __asm__(TIMER1_COUNTER_ASM);
 */

#define TIMER1_COUNTER_ASM "D_8009CC1C"
#define TIMER1_MODE_ASM "D_8009DD54"

#endif /* GAME_TIMER_GLOBALS_H */
