#ifndef GAME_DMA_GLOBALS_H
#define GAME_DMA_GLOBALS_H

/*
 * DMA controller register pointers.
 *
 * Retail keeps the DMA register addresses in writable pointer variables and
 * writes through them. Each pointer keeps its retail assembler symbol through
 * an asm label, so the generated machine code is unchanged.
 *
 * The pointers are declared where they are used, because the units need
 * different qualification; a single shared type changes code generation.
 * Bind the declaration with the labels below, for example:
 *
 *   extern volatile u32 *gDma2BlockCtrlPtr __asm__(DMA2_BLOCK_CTRL_ASM);
 *
 * Register map (public PlayStation documentation):
 *   0x1F8010A0 DMA2 memory address   (GPU)
 *   0x1F8010A4 DMA2 block control    (block size / block count)
 *   0x1F8010A8 DMA2 channel control  (direction, step, sync mode, start)
 */

#define DMA2_BLOCK_CTRL_ASM "D_8009DED0"

#endif /* GAME_DMA_GLOBALS_H */
