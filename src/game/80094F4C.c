// Categories: Platform > Hardware Init
#include "game/dma_globals.h"

#include "game/gpu_globals.h"

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

extern volatile u32 gGpuJobTail __asm__("D_8009DEEC");
extern u32 gGpuJobHead __asm__("D_8009DEE8");
extern u32 gSavedIrqMask __asm__("D_8009DEF8");
extern u32 *gDma2ChannelCtrlPtr __asm__("D_8009DED4");
extern u32 *gDmaControlGpuPtr __asm__("D_8009DEE4");
extern u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");
extern u8 gGpuLibPair[] __asm__("D_800A62C8");
extern u8 gGpuDmaQueue[] __asm__("D_800A9450");

extern u32 gSwapIrqMask(u32 a0) __asm__("func_800915FC");
extern u32 func_80095350(u32 a0);
extern void gMemset(unsigned char *destination, int value, unsigned int count) __asm__("func_80095830");
extern void func_800951D8(void);
extern void gWaitImageTransferFrame(void) __asm__("func_80094CEC");
extern s32 func_8009520C(void);

/**
 * @brief Programs the video/transfer registers and clears the working buffers.
 *
 * The previous register value is saved with func_800915FC(0) and restored at
 * the end. Three status words are cleared (D_8009DEEC and the copy in
 * D_8009DEE8, plus D_8009DEF8 which holds the saved value). Mode bits 0, 5 and
 * 1, 3 then choose between two register setups: modes 0/5 write 0x401 through
 * D_8009DED4, raise bit 0x800 in D_8009DEE4, clear D_8009DEC8 and zero 0x100
 * and 0x1800 bytes of the two buffers; modes 1/3 write the same 0x401, raise
 * the same bit and write 0x2000000 followed by 0x1000000 through D_8009DEC8.
 * Only mode 0 continues into func_80095350.
 *
 * @param a0 Mode selector, the low three bits are used.
 * @return The result of func_80095350 for mode 0, otherwise 0.
 *
 * @par Globals
 * D_8009DED4, D_8009DEE4, D_8009DEC8 - three aliases of hardware registers;
 * D_8009DEEC/D_8009DEE8/D_8009DEF8 - saved status words;
 * D_800A62C8 (0x100 B) and D_800A9450 (0x1800 B) - working buffers.
 *
 * @par Control flow
 * Save, switch over the mode pattern, restore, then one continuation call.
 *
 * @par Calls
 * func_800915FC - read/write the saved register. func_80095830 - buffer fill.
 * func_80095350 - follow-up setup for mode 0.
 *
 * @par Side effects
 * Hardware register writes and two buffer clears.
 *
 * @par Callers
 * Confirmed: func_80092910.
 */
s32 func_80094F4C(s32 a0)
{
    u32 saved;

    saved = gSwapIrqMask(0);
    gGpuJobTail = 0;
    gGpuJobHead = gGpuJobTail;
    gSavedIrqMask = saved;

    switch (a0 & 7) {
    case 0:
    case 5:
        *gDma2ChannelCtrlPtr = 0x401;
        *gDmaControlGpuPtr |= 0x800;
        *gGpuGp1PortPtr = 0;
        gMemset(gGpuLibPair, 0, 0x100);
        gMemset(gGpuDmaQueue, 0, 0x1800);
        break;
    case 1:
    case 3:
        *gDma2ChannelCtrlPtr = 0x401;
        *gDmaControlGpuPtr |= 0x800;
        *gGpuGp1PortPtr = 0x2000000;
        *gGpuGp1PortPtr = 0x1000000;
        break;
    }

    gSwapIrqMask(gSavedIrqMask);
    if ((a0 & 7) == 0) {
        return func_80095350(a0);
    }
    return 0;
}
