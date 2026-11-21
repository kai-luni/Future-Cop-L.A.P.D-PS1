// Categories: Platform > GPU | Graphics > VRAM Transfer
#include "game/types.h"

typedef struct {
    u8 pad00[8];
    void (*callback)(s32 arg0, void *arg1, s32 arg2, void *arg3);
    u8 pad0C[0x14];
    s32 unk20;
} DispatchDDE8;

extern u8 gDebugTagRect[] __asm__("D_80012A94");
extern DispatchDDE8 *gVramWriteCallback __asm__("D_8009DDE8");
extern void gValidateGpuRectDebug(void *arg0, void *arg1) __asm__("func_80092CF4");
void gLoadImage(void *rect, void *data) __asm__("func_80092F38");


/**
 * @brief VRAM transfer (LoadImage/StoreImage style): copies a rectangle
 *        between main memory and the frame buffer.
 *
 * @param rect Rectangle descriptor (x, y, w, h as four half-words).
 * @param data Source/destination block in main memory.
 *
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * func_80092CF4(D_80012A94, rect) performs the debug/log wrapper, then the GPU
 * library callback at D_8009DDE8+8 is invoked as
 * callback(state_word_at_0x20, rect, 8, data) - the trailing 8 is the transfer
 * mode word.
 *
 * @par Globals
 * D_8009DDE8 (GPU library state), D_80012A94 (debug format string).
 *
 * @par Callers
 * func_8001B4A8 (deferred transfer queue), func_8003FAD4, func_80085074
 * (decoded frame output) and gPlayVideoFile, as well as the texture load paths
 * func_8005A414 ("Cpyr") and func_80063C5C ("Cbmp").
 *
 * @par Texture load path
 * Primary VRAM rectangle upload primitive called by func_8005A414
 * ("Cpyr") and func_80063C5C ("Cbmp") (each paired with func_80092C8C) to
 * transfer unpacked pixel and CLUT rectangles into VRAM via D_8009DDE8+8.
 *
 * @par Environment path
 * Transfers a rectangular pixel buffer from main RAM to VRAM by checking
 * clipping bounds and issuing an 8-word GPU DMA transfer command via
 * func_80092CF4 (called by func_8001B4A8, func_8003FAD4, func_80048BF8).
 */
void gLoadImage(void *rect, void *data)
{
    gValidateGpuRectDebug(gDebugTagRect, rect);
    gVramWriteCallback->callback(gVramWriteCallback->unk20, rect, 8, data);
}
