// Categories: Platform > Controller Input | Input > Analog
#include "game/types.h"

struct PadBuffer {
    u8 unk00[4];
    u8 rightX;
    u8 rightY;
    u8 leftX;
    u8 leftY;
    u8 unk08[0x1C];
};

extern struct PadBuffer gPadBuffers[] __asm__("D_800A78F0");

extern s32 func_8001EF34(s32 port);

/**
 * @brief Reads raw left or right analog-stick byte coordinates from a pad buffer.
 *
 * Verifies that the port holds controller type 7 (analog pad) via func_8001EF34,
 * then copies either the right stick (stick == 1, +0x04/+0x05) or left stick
 * (+0x06/+0x07) bytes from D_800A78F0[port] into *x and *y.
 *
 * @param x Output pointer receiving the 8-bit horizontal stick value.
 * @param y Output pointer receiving the 8-bit vertical stick value.
 * @param port Controller port index into D_800A78F0.
 * @param stick Stick selector: 1 selects rightX/rightY, any other value selects leftX/leftY.
 * @return 1 if func_8001EF34(port) == 7 and coordinates were written, else 0.
 *
 * @par Globals
 * D_800A78F0 - per-port PadBuffer array storing raw controller packets.
 *
 * @par Control flow
 * Early return 0 when func_8001EF34(port) != 7; otherwise branches on stick == 1 to select right or left axis pair and returns 1.
 *
 * @par Calls
 * func_8001EF34 - checks pad buffer status byte and returns controller type nibble (4 or 7).
 *
 * @par Side effects
 * Writes *x and *y when controller type 7 is detected.
 *
 * @par Callers
 * Confirmed: func_800538C8 (0x8005392C, converts left stick axes into direction flags and magnitudes).
 */
s32 func_8001E7B8(s32 *x, s32 *y, s32 port, s32 stick)
{
    if (func_8001EF34(port) != 7) {
        return 0;
    }
    if (stick == 1) {
        *x = gPadBuffers[port].rightX;
        *y = gPadBuffers[port].rightY;
    } else {
        *x = gPadBuffers[port].leftX;
        *y = gPadBuffers[port].leftY;
    }
    return 1;
}
