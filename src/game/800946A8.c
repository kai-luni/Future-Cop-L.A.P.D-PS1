// Categories: Graphics > GPU Transfer | Platform > DMA
#include "game/dma_globals.h"

#include "game/gpu_globals.h"

typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;

extern s16 gMaxDrawWidth __asm__("D_8009DDF4");
extern s16 gMaxDrawHeight __asm__("D_8009DDF6");
extern volatile u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");
extern volatile u32 *gGpuGp0PortPtr __asm__("D_8009DEC4");
extern volatile u32 *gDma2MemAddrPtr __asm__("D_8009DECC");
extern volatile u32 *gDma2BlockCtrlPtr __asm__("D_8009DED0");
extern volatile u32 *gDma2ChannelCtrlPtr __asm__("D_8009DED4");
extern void gArmGpuDmaTimeout(void) __asm__("func_800951D8");
extern int gCheckGpuDmaTimeout(void) __asm__("func_8009520C");

typedef struct {
    u32 w0;
    s16 x;
    s16 y;
} Hdr;
int gReadGpuVramRectDma(Hdr *arg0, u32 *out) __asm__("func_800946A8");


/**
 *  @brief Starts a GPU image readback into an output buffer.
 *
 *  Clamps the header dimensions, sends the readback command, reads remainder
 *  words directly, and starts DMA for any complete 16-word blocks.
 *  @param arg0 Image command header whose dimensions are clamped in place.
 *  @param out Destination for the transferred 32-bit words.
 *  @return 0 on setup success, or -1 for an empty transfer or polling failure.
 *  @par Globals
 *  D_8009DDF4 and D_8009DDF6 bound dimensions; D_8009DEC4 and D_8009DEC8
 *  address GPU registers; D_8009DECC, D_8009DED0, and D_8009DED4 address DMA registers.
 *  @par Control flow
 *  Polls GPU readiness with failure checks, reads remainder words in a loop,
 *  and conditionally starts DMA for complete blocks.
 *  @par Calls
 *  func_800951D8 prepares the operation; func_8009520C checks polling failure.
 *  @par Side effects
 *  Modifies the header and output buffer and writes GPU and DMA registers.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
int gReadGpuVramRectDma(Hdr *arg0, u32 *out)
{
    register Hdr *p __asm__("$17");
    register int n __asm__("$3");
    int half;
    int rem;
    int blocks;
    int t;

    gArmGpuDmaTimeout();
    p = arg0;
    p->x = (p->x < 0) ? 0 : ((gMaxDrawWidth < p->x) ? (u16)gMaxDrawWidth : (u16)p->x);
    p->y = (p->y < 0) ? 0 : ((gMaxDrawHeight < p->y) ? (u16)gMaxDrawHeight : (u16)p->y);
    n = p->x * p->y + 1;
    half = n / 2;
    rem = half >> 4;
    if (half <= 0) return -1;
    t = rem;
    rem = half - (t << 4);
    blocks = t;
    if (!(*gGpuGp1PortPtr & 0x4000000)) {
        __asm__ volatile("" ::: "$3");
        do {
            if (gCheckGpuDmaTimeout()) return -1;
        } while (!(*gGpuGp1PortPtr & 0x4000000));
    }
    *gGpuGp1PortPtr = 0x4000000;
    *gGpuGp0PortPtr = 0x1000000;
    *gGpuGp0PortPtr = 0xC0000000;
    *gGpuGp0PortPtr = p->w0;
    *gGpuGp0PortPtr = ((u32 *)p)[1];
    if (!(*gGpuGp1PortPtr & 0x8000000)) {
        __asm__ volatile("" ::: "$3");
        do {
            if (gCheckGpuDmaTimeout()) return -1;
        } while (!(*gGpuGp1PortPtr & 0x8000000));
    }
    {
        int m;
        rem--;
        __asm__("li %0,-1" : "=r"(m));
        if (rem != m) {
            do {
                *out++ = *gGpuGp0PortPtr;
            } while (--rem != -1);
        }
    }
    if (blocks) {
        *gGpuGp1PortPtr = 0x4000003;
        *gDma2MemAddrPtr = (u32)out;
        *gDma2BlockCtrlPtr = (blocks << 16) | 0x10;
        *gDma2ChannelCtrlPtr = 0x1000200;
    }
    return 0;
}
