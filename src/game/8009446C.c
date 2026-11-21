// Categories: Graphics > Image Transfer | Platform > DMA
#include "game/dma_globals.h"

#include "game/gpu_globals.h"

typedef unsigned int u32;
typedef unsigned short u16;
typedef short s16;

extern u16 gMaxDrawHeight __asm__("D_8009DDF6");
extern u16 gMaxDrawWidth __asm__("D_8009DDF4");
extern volatile u32 *gDma2ChannelCtrlPtr __asm__("D_8009DED4");
extern volatile u32 *gDma2MemAddrPtr __asm__("D_8009DECC");
extern volatile u32 *gDma2BlockCtrlPtr __asm__("D_8009DED0");
extern volatile u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");
extern u32 *volatile gGpuGp0PortPtr __asm__("D_8009DEC4");
extern void gArmGpuDmaTimeout(void) __asm__("func_800951D8");
extern int gCheckGpuDmaTimeout(void) __asm__("func_8009520C");
int gUploadImageToGpu(u16 *hdr_, u32 *data_) __asm__("func_8009446C");


/**
 *  @brief Sends an image header and pixel data to the graphics hardware.
 *
 *  Clamps the header dimensions, waits for readiness, and sends initial words
 *  directly before starting DMA for any remaining 16-word blocks.
 *  @param hdr_ Four-halfword image header whose dimensions are clamped in place.
 *  @param data_ Source of packed pixel words.
 *  @return 0 after issuing the transfer, or -1 for an empty transfer or wait failure.
 *  @par Globals
 *  D_8009DDF4 and D_8009DDF6 limit dimensions; D_8009DEC4 and D_8009DEC8 through D_8009DED4 address graphics and DMA registers.
 *  @par Control flow
 *  Rejects nonpositive word counts, polls readiness, writes remainder words, and conditionally starts DMA.
 *  @par Calls
 *  func_800951D8 prepares the transfer; func_8009520C checks for failure while polling.
 *  @par Side effects
 *  Modifies the header and writes graphics and DMA registers; DMA may continue after return.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Environment path
 * Uploads a pixel rectangle to GPU VRAM via GP0 and DMA2. Calls
 * func_800951D8, clamps width/height to D_8009DDF4/D_8009DDF6, polls
 * D_8009DEC8 bit 0x4000000 with func_8009520C, sends GP0 0xA0000000 plus the
 * clamped rect header, writes unaligned words to D_8009DEC4, and launches
 * DMA2 for 16-word blocks.
 */
int gUploadImageToGpu(u16 *hdr_, u32 *data_)
{
    u32 *data = data_;
    u16 *hdr = hdr_;
    int dir = 0;
    int rem, tmp;
    int blocks;
    u16 a; int n, words, t;
    s16 sv; u16 uv; s16 sv2; u16 uv2; u16 b;
    u32 first;

    gArmGpuDmaTimeout();
    sv = hdr[2]; uv = hdr[2];
    if (sv >= 0) {
        a = uv;
        if ((s16)gMaxDrawWidth < sv) a = gMaxDrawWidth;
    } else {
        a = 0;
    }
    hdr[2] = a;
    sv2 = hdr[3]; uv2 = hdr[3];
    if (sv2 >= 0) {
        b = uv2;
        if ((s16)gMaxDrawHeight < sv2) b = gMaxDrawHeight;
    } else {
        b = 0;
    }
    hdr[3] = b;
    n = (s16)hdr[2] * (s16)hdr[3];
    t = n + 1;
    words = t / 2;
    if (words <= 0) return -1;
    rem = words >> 4; tmp = rem; rem = words - tmp * 16; blocks = tmp;
    if ((*gGpuGp1PortPtr & 0x4000000) == 0) {
        __asm__ volatile("" ::: "$3");
        do {
            if (gCheckGpuDmaTimeout() != 0) return -1;
        } while ((*gGpuGp1PortPtr & 0x4000000) == 0);
    }
    *gGpuGp1PortPtr = 0x4000000;
    *(volatile u32 *)gGpuGp0PortPtr = 0x1000000;
    *(volatile u32 *)gGpuGp0PortPtr = dir ? 0xB0000000 : 0xA0000000;
    *(volatile u32 *)gGpuGp0PortPtr = *(u32 *)hdr;
    *(volatile u32 *)gGpuGp0PortPtr = ((u32 *)hdr)[1];
    rem--;
    if (rem != -1) {
        __asm__ volatile("" ::: "$2");
        do {
            *gGpuGp0PortPtr = *data++;
        } while (--rem != -1);
    }
    if (blocks != 0) {
        *gGpuGp1PortPtr = 0x4000002;
        *gDma2MemAddrPtr = (u32)data;
        *gDma2BlockCtrlPtr = (blocks << 16) | 0x10;
        *gDma2ChannelCtrlPtr = 0x1000201;
    }
    return 0;
}
