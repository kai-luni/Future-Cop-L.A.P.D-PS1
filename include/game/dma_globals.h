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
 *   extern volatile u32 *gDma2MemAddrPtr __asm__(DMA2_MEM_ADDR_ASM);
 *   extern volatile u32 *gDma2BlockCtrlPtr __asm__(DMA2_BLOCK_CTRL_ASM);
 *
 * Register map (public PlayStation documentation):
 *   0x1F8010A0 DMA2 memory address   (GPU)
 *   0x1F8010A4 DMA2 block control    (block size / block count)
 *   0x1F8010A8 DMA2 channel control  (direction, step, sync mode, start)
 *   0x1F801088 DMA0 channel control (MDEC output)
 *   0x1F801098 DMA1 channel control (MDEC input)
 *   0x1F8010B8 DMA3 channel control (CD-ROM)
 *   0x1F8010F0 DMA control (per-channel priority and enable); retail holds
 *              three pointers to it: D_8009DD1C, D_8009DEE4, D_8009EEB4
 *   0x1F8010F4 DMA interrupt control (channel enables, channel flags)
 */

#define DMA_CONTROL_GPU_ASM "D_8009DEE4"
#define DMA_CONTROL_CDROM_ASM "D_8009CBA4"
#define DMA_CONTROL_MDEC_ASM "D_8009E9F8"
#define DMA_CONTROL_ASM "D_8009DD1C"
#define DMA_INTERRUPT_CTRL_ASM "D_8009DD60"
#define DMA_CHANNEL_BASE_ASM "D_8009DD84"
#define DMA0_MEM_ADDR_ASM "D_8009E9C0"
#define DMA0_BLOCK_CTRL_ASM "D_8009E9C4"
#define DMA0_CHANNEL_CTRL_ASM "D_8009E9C8"
#define DMA1_MEM_ADDR_ASM "D_8009E9CC"
#define DMA1_BLOCK_CTRL_ASM "D_8009E9D0"
#define DMA1_CHANNEL_CTRL_ASM "D_8009E9D4"
#define DMA3_MEM_ADDR_ASM "D_8009CBA8"
#define DMA3_BLOCK_CTRL_ASM "D_8009CBAC"
#define DMA3_CHANNEL_CTRL_ASM "D_8009CBB0"
#define DMA4_MEM_ADDR_ASM "D_8009EEA8"
#define DMA4_BLOCK_CTRL_ASM "D_8009EEAC"
#define DMA4_CHANNEL_CTRL_ASM "D_8009EEB0"
#define DMA6_MEM_ADDR_ASM "D_8009DED8"
#define DMA6_BLOCK_CTRL_ASM "D_8009DEDC"
#define DMA6_CHANNEL_CTRL_ASM "D_8009DEE0"
#define DMA2_MEM_ADDR_ASM "D_8009DECC"
#define DMA2_BLOCK_CTRL_ASM "D_8009DED0"
#define DMA2_CHANNEL_CTRL_ASM "D_8009DED4"

#endif /* GAME_DMA_GLOBALS_H */
