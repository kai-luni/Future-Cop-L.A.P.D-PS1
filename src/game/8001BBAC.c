// Categories: Game Engine > Frame | Graphics > Display
#include "game/types.h"

extern s32 gSubScreenActive __asm__("D_8009F5C0");

extern void func_8001B4A8(s32 arg0);
void gEndSubScreenFrame(void) __asm__("func_8001BBAC");


/**
 * @brief Ends the frame in mode 0 and toggles the frame parity counter.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * func_8001B4A8(0) (frame end), then D_8009F5C0 increments and wraps
 * 2 -> 0, giving a 0/1 alternation.
 *
 * @par Likely role
 * The parity drives double-buffer or interlace alternation in the frame
 * path.
 *
 * @par Callers
 * func_80027510.
 *
 * @par Role and path
 * Ends a sub-screen frame by calling func_8001B4A8(0) and then increments
 * gSubScreenActive (D_8009F5C0), wrapping from 2 back to 0 for 0/1 parity
 * alternation. Driven by callers func_80027510 and func_800271A4 during
 * split-screen or dual-screen frame submission.
 */
void gEndSubScreenFrame(void)
{
    func_8001B4A8(0);
    if (++gSubScreenActive >= 2) {
        gSubScreenActive = 0;
    }
}
