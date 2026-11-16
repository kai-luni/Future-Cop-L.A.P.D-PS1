// Categories: Platform > CD-ROM > Streaming | Platform > Error Handling
#include "game/dma_globals.h"

#include "game/gpu_globals.h"

typedef unsigned int u32;
extern int D_8009DEFC, D_8009DF00;
extern volatile u32 D_8009DEEC;
extern u32 D_8009DEE8, D_8009DEF8;
extern volatile u32 *D_8009DED4, *D_8009DEE4;
extern volatile u32 *gDma2MemAddrPtr __asm__("D_8009DECC");
extern volatile u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");
extern char D_80012B48[];
extern int func_80091270(int);
extern u32 func_800915FC(u32);
extern void func_8008E000(char *, u32, u32, u32, u32);

/**
 * @brief Streaming watchdog: detects a stalled transfer, dumps the ring
 *        state and resets the DMA channel.
 *
 * @return 0 while healthy, -1 after a stall was handled.
 *
 * @par Confirmed
 * A stall is D_8009DEFC < func_80091270(-1) or the poll counter
 * D_8009DF00 exceeding 0xF0000. The fail path prints via func_8008E000
 * (message D_80012B48, the ring fill (D_8009DEE8 - D_8009DEEC) & 0x3F and
 * three port words), then under func_800915FC(0) interrupt masking:
 * D_8009DEEC = 0, D_8009DEF8 = the saved mask, D_8009DEE8 = 0,
 * *D_8009DED4 = 0x401, *D_8009DEE4 |= 0x800, *D_8009DEC8 = 0x2000000 then
 * 0x1000000 (stop/start), mask restored.
 *
 * @par Likely role
 * The XA/CD streaming health check called by every transfer wait loop
 * (the func_80094xxx/func_80095xxx family): the 0xF0000 poll cap bounds
 * the busy-wait, the reset recovers a hung DMA without a full re-init.
 *
 * @par Unknown
 * The exact DMA register identities behind the four port pointers.
 *
 * @par Callers
 * func_8009415C, func_8009446C, func_800946A8, func_80094A3C, func_8009509C,
 * func_800953F0, func_800954DC, func_800955C8, func_8009570C.
 */
int func_8009520C(void)
{
    u32 saved;
    register u32 count __asm__("$5");
    register int *counter __asm__("$3");
    register volatile u32 *port __asm__("$6");
    register char *message __asm__("$4");
    if (D_8009DEFC < func_80091270(-1)) goto fail;
    __asm__("lui %0,%%hi(D_8009DF00)\n\taddiu %0,%0,%%lo(D_8009DF00)" : "=r"(counter));
    if ((*counter)++ > 0xF0000) {
fail:
        __asm__ volatile("lui %0,%%hi(D_8009DEC8)\n\tlw %0,%%lo(D_8009DEC8)(%0)\n\tlui %1,%%hi(D_80012B48)\n\taddiu %1,%1,%%lo(D_80012B48)\n\tlw $2,0(%0)" : "=r"(port), "=r"(message));
        count = D_8009DEE8;
        func_8008E000(message, (count - D_8009DEEC) & 0x3f, *port, *D_8009DED4, *gDma2MemAddrPtr);
        saved = func_800915FC(0);
        D_8009DEEC = 0;
        D_8009DEF8 = saved;
        D_8009DEE8 = D_8009DEEC;
        *D_8009DED4 = 0x401;
        *D_8009DEE4 |= 0x800;
        *gGpuGp1PortPtr = 0x2000000;
        *gGpuGp1PortPtr = 0x1000000;
        func_800915FC(D_8009DEF8);
        return -1;
    }
    return 0;
}
