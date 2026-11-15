// Categories: Game Engine > Frame | Graphics > Display
#include "game/types.h"

extern void func_80024EC8(void);
extern void func_8001B4A8(s32 arg0);

/**
 * @brief Runs the asset pass and then presents the frame.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * func_80024EC8() (the "graphics assets of the loaded level" pass) followed by
 * func_8001B4A8(1), the frame-end routine that sorts depth buckets, builds the
 * draw list and swaps the buffers.
 *
 * @par Likely role
 * The per-frame step of a screen that shows level geometry, i.e. exactly the
 * two calls a scene renderer needs.
 *
 * @par Callers
 * func_800287F4 (the screen dispatcher).
 */
void func_80027EBC(void)
{
    (void)func_80024EC8();
    func_8001B4A8((s32)0x1);
}
