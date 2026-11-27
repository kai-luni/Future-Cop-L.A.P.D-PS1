// Categories: Platform > GPU | Game Engine > Command Queue
#include "game/types.h"

extern void gSubmitGpuDmaJob(s32, s32, s32, s32) __asm__("func_80094A3C");
void gQueueDirectGpuDmaJob(s32 arg0, s32 arg1, s32 arg2) __asm__("func_80094A18");


/**
 * @brief Queues or executes a direct-pointer GPU/DMA2 callback job with zero copy size.
 *
 * Adapter calling func_80094A3C(arg0, arg1, 0, arg2) so the 64-entry GPU/DMA2 ring D_800A9450 passes arg1 directly without copying inline payload bytes; its address sits at 0x8009DDAC in table D_8009DDA8.
 *
 * @param arg0 Job callback function pointer passed as func to func_80094A3C.
 * @param arg1 Direct payload pointer forwarded as arg1 to func_80094A3C (uncopied).
 * @param arg2 Secondary context value forwarded as arg3 to func_80094A3C.
 *
 * @par Globals
 * None directly; address is referenced in callback table D_8009DDA8 (+0x04 at 0x8009DDAC).
 *
 * @par Control flow
 * Straight-line rearrangement of arguments with 0 copy length and call to func_80094A3C.
 *
 * @par Calls
 * func_80094A3C - runs or enqueues (arg0, arg1, 0, arg2) on the D_800A9450 GPU/DMA2 queue.
 *
 * @par Side effects
 * Executes or queues GPU/DMA2 work in D_800A9450 via func_80094A3C.
 *
 * @par Callers
 * Confirmed: none direct; address taken at 0x8009DDAC inside D_8009DDA8 (passed to func_8008E000/func_80092910) / Likely: indirect dispatch via D_8009DDA8 / Unknown: exact indirect call site.
 */
void gQueueDirectGpuDmaJob(s32 arg0, s32 arg1, s32 arg2)
{
    gSubmitGpuDmaJob(arg0, arg1, 0, arg2);
}
