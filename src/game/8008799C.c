// Categories: Resources > Memory Management | Resources > Movie Playback
#include "game/types.h"

void func_80017EBC(s32 arg0, s32 arg1);

/**
 * @brief Allocates a movie playback buffer of arg0 bytes via mode-2 free-range search.
 *
 * Calls func_80017EBC(arg0, 2); although typed void in C, gOpenVideoLayer and gPlayVideoFile use the pointer returned in $v0 for video decode, stream ring, and YUV/RGB buffers (D_8009F4E4, D_8009F4FC, D_8009F500, gMovieStreamBuffer, gMovieAudioBlock).
 *
 * @param arg0 Buffer size in bytes passed to func_80017EBC.
 *
 * @par Globals
 * None directly.
 *
 * @par Control flow
 * Single call to func_80017EBC(arg0, 2) and return (leaving allocated pointer in $v0).
 *
 * @par Calls
 * func_80017EBC - allocates arg0 bytes from the free-range allocator (mode 2).
 *
 * @par Side effects
 * Allocates memory in the tag-2 free-range heap via func_80017EBC.
 *
 * @par Callers
 * Confirmed: gOpenVideoLayer (0x80087F5C, 0x80087F70, 0x80087F84, 0x80087FF0, 0x80088064, 0x800880A4 in asm/us/main.s), gPlayVideoFile (0x80088220, 0x80088234, 0x80088508, 0x8008851C in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_8008799C(s32 arg0) {
    func_80017EBC(arg0, 2);
    return;
}
