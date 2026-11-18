// Categories: Graphics > Video Decode | Platform > MDEC | Resources > Movie Playback
#include "game/types.h"

extern u8 *gMovieSliceContext __asm__("D_8009F4BC");
extern void gMovieSliceFeedDma(void) __asm__("func_80085228");
extern void gMovieSliceFeedRows(void) __asm__("func_80085630");

/**
 * @brief Dispatches movie frame-slice decoding/upload to the async or sync MDEC path.
 *
 * Inspects *(s32 *)(gMovieSliceContext + 0x12C); if non-zero, calls gMovieSliceFeedDma (interrupt-driven
 * upload), otherwise calls gMovieSliceFeedRows (synchronous upload), passing $a0..$a2 untouched.
 *
 * @param void Declared void in C; forwards caller registers $a0 (slice), $a1 (size), $a2 (offset) unchanged.
 *
 * @par Globals
 * gMovieSliceContext - video playback context pointer (reads word at +0x12C as mode flag).
 *
 * @par Control flow
 * Single conditional branch on *(s32 *)(gMovieSliceContext + 0x12C) != 0 selecting between two callees.
 *
 * @par Calls
 * gMovieSliceFeedDma - interrupt-driven video slice decoder/uploader; gMovieSliceFeedRows - synchronous slice uploader.
 *
 * @par Side effects
 * Decodes and uploads a movie frame slice and updates gMovieSliceContext buffer state via the selected callee.
 *
 * @par Callers
 * Confirmed: gMovieStreamDemux (0x80087CB8, passes (data, arg0, arg1) on MDEC/MDC2 chunks) / Likely: none / Unknown: none.
 */
void gMovieDecodeSlice(void) __asm__("func_8008589C");

void gMovieDecodeSlice(void)
{
    if (*(s32 *)(gMovieSliceContext + 0x12C) != 0) {
        gMovieSliceFeedDma();
    } else {
        gMovieSliceFeedRows();
    }
}
