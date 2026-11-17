// Categories: Graphics > Video Decode | Platform > MDEC | Resources > Movie Playback
#include "game/types.h"

extern u8 *gDecodeJob __asm__("D_8009F4BC");
extern void func_80085228(void);
extern void func_80085630(void);

/**
 * @brief Dispatches movie frame-slice decoding/upload to the async or sync MDEC path.
 *
 * Inspects *(s32 *)(D_8009F4BC + 0x12C); if non-zero, calls func_80085228 (interrupt-driven
 * upload), otherwise calls func_80085630 (synchronous upload), passing $a0..$a2 untouched.
 *
 * @param void Declared void in C; forwards caller registers $a0 (slice), $a1 (size), $a2 (offset) unchanged.
 *
 * @par Globals
 * D_8009F4BC - video playback context pointer (reads word at +0x12C as mode flag).
 *
 * @par Control flow
 * Single conditional branch on *(s32 *)(D_8009F4BC + 0x12C) != 0 selecting between two callees.
 *
 * @par Calls
 * func_80085228 - interrupt-driven video slice decoder/uploader; func_80085630 - synchronous slice uploader.
 *
 * @par Side effects
 * Decodes and uploads a movie frame slice and updates D_8009F4BC buffer state via the selected callee.
 *
 * @par Callers
 * Confirmed: func_800879DC (0x80087CB8, passes (data, arg0, arg1) on MDEC/MDC2 chunks) / Likely: none / Unknown: none.
 */
void func_8008589C(void)
{
    if (*(s32 *)(gDecodeJob + 0x12C) != 0) {
        func_80085228();
    } else {
        func_80085630();
    }
}
