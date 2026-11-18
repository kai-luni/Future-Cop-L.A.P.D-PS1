// Categories: Graphics > Video Layer | Resources > Memory Management
#include "game/types.h"

extern void func_800181D4(void);

/**
 * @brief Releases the video-decode buffer through the top allocator.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * One tail call to func_800181D4 (the mode-aware free of the top allocator).
 * Which buffer is freed is decided by func_800181D4 itself, which reads the
 * current allocation mode from D_8009FAxx.
 *
 * @par Callers
 * The movie/image playback path.
 */
void gMovieFreeDecodeBuffer(void) __asm__("func_800879BC");

void gMovieFreeDecodeBuffer(void)
{
    func_800181D4();
}
