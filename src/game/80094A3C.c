// Categories: Platform > GPU | Game Engine > Command Queue
#include "game/dma_globals.h"

#include "game/gpu_globals.h"

#include "game/types.h"

struct QueueEntry {
    void (*func)();
    void *arg1;
    s32 arg2;
    u32 data[21];
};

struct StructDDF0 {
    u8 unk0;
    u8 unk1;
    u8 pad[6];
    s32 unk8;
    s32 unkC;
};

extern volatile struct QueueEntry gGpuDmaQueue[] __asm__("D_800A9450");
extern struct StructDDF0 gDebugTagBytes __asm__("D_8009DDF0");
extern volatile s32 gGpuJobHead __asm__("D_8009DEE8");
extern volatile s32 gGpuJobTail __asm__("D_8009DEEC");
extern u32 gDispatchSavedIrqMask __asm__("D_8009DEF0");
extern volatile u32 *gDma2ChannelCtrlPtr __asm__("D_8009DED4");
extern volatile u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");

extern void func_800951D8(void);
extern s32 func_8009520C(void);
extern void gWaitImageTransferFrame(void) __asm__("func_80094CEC");
extern u32 func_800915FC(u32);
extern void func_800914E0(s32, void (*)(void));

/**
 * @brief Enqueues or immediately executes a GPU/DMA2 callback job in the 64-entry ring D_800A9450.
 *
 * Arms timeout via func_800951D8, drains full ring via func_80094CEC (returning -1 if func_8009520C stalls), masks interrupts, and either runs func(arg1, arg3) directly when GPU DMA2 is idle (unmasked unk1 == 0 or empty ring + idle CHCR/unkC) after GPUSTAT bit 26 ready, or queues it in D_800A9450 with DMA2 IRQ callback func_80094CEC.
 *
 * @param func Job callback invoked as func(arg1, arg3).
 * @param arg1 Payload pointer or inline buffer to copy.
 * @param arg2 Byte count to copy into D_800A9450[head].data (0 passes arg1 directly).
 * @param arg3 Second argument forwarded to func.
 * @return 0 if executed immediately, queue count ((head - tail) & 0x3F) if queued, or -1 on GPU timeout.
 *
 * @par Globals
 * D_800A9450 - 64-entry job ring; D_8009DEE8/D_8009DEEC - head/tail indices; D_8009DDF0/D_8009DEF0 - queue/IRQ state; D_8009DEC8/D_8009DED4 - GPUSTAT/DMA2_CHCR ptrs.
 * @par Control flow
 * Full-ring wait loop, direct-dispatch vs queued branch, optional word-copy loop into entry.data, and queue drain trigger.
 * @par Calls
 * func_800951D8 - arms timeout; func_8009520C - checks timeout; func_80094CEC - drains ring; func_800915FC - saves/restores IRQ mask; func_800914E0 - sets DMA2 callback.
 * @par Side effects
 * Updates D_8009DDF0, D_8009DEF0, D_800A9450, D_8009DEE8, installs DMA2 callback, and invokes func.
 * @par Callers
 * Confirmed: func_80094A18 (0x80094A24) / Likely: indirect via jump/dispatch table at 0x8009DDB0 / Unknown: none.
 */
s32 func_80094A3C(void (*func)(), void *arg1, s32 arg2, s32 arg3)
{
    s32 i;
    u32 *src;
    register volatile u32 (*payload)[24] __asm__("$8");
    register s32 offset __asm__("$4");
    register void *copy __asm__("$2");
    register void **dest __asm__("$1");
    register struct StructDDF0 *ddf0 __asm__("$4");

    func_800951D8();

    while (((gGpuJobHead + 1) & 0x3F) == gGpuJobTail) {
        if (func_8009520C() != 0) {
            return -1;
        }
        gWaitImageTransferFrame();
    }

    gDispatchSavedIrqMask = func_800915FC(0);
    ddf0 = &gDebugTagBytes;
    __asm__("" : "=r"(ddf0) : "0"(ddf0));
    ddf0->unk8 = 1;

    if (ddf0->unk1 == 0 || (gGpuJobHead == gGpuJobTail && (*gDma2ChannelCtrlPtr & 0x1000000) == 0 && ddf0->unkC == 0)) {
        while ((*gGpuGp1PortPtr & 0x04000000) == 0) {
        }
        func(arg1, arg3);
        func_800915FC(gDispatchSavedIrqMask);
        return 0;
    }

    func_800914E0(2, gWaitImageTransferFrame);

    i = 0;
    if (arg2 != 0) {
        payload = (volatile u32 (*)[24])gGpuDmaQueue[0].data;
        __asm__("" : "=r"(payload) : "0"(payload));
        for (src = (u32 *)arg1; i < arg2 / 4; i++) {
            payload[gGpuJobHead][i] = *src++;
        }
        offset = gGpuJobHead * 96;
        copy = (void *)gGpuDmaQueue[gGpuJobHead].data;
        __asm__ volatile("lui %0,%%hi(D_800A9450+4)\n\taddu %0,%0,%1" : "=r"(dest) : "r"(offset));
        *(void **)((u8 *)dest - 0x6BAC) = copy;
    } else {
        gGpuDmaQueue[gGpuJobHead].arg1 = arg1;
    }

    gGpuDmaQueue[gGpuJobHead].arg2 = arg3;
    gGpuDmaQueue[gGpuJobHead].func = func;
    __asm__ volatile("" ::: "memory");

    gGpuJobHead = (gGpuJobHead + 1) & 0x3F;
    func_800915FC(gDispatchSavedIrqMask);
    gWaitImageTransferFrame();

    i = (gGpuJobHead - gGpuJobTail) & 0x3F;
    __asm__ volatile(".set\tnoreorder\n\t.set reorder");
    return i;
}
