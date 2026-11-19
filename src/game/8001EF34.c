// Categories: Game Engine > Input > Controller
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
s32 gGetPadType(s32 port) __asm__("func_8001EF34");


/**
 * @brief Queries the controller type of a port, accepting only two pad ids.
 *
 * @param port Controller port index into D_800A78F0.
 * @return The pad type nibble (4 or 7), or 0 when absent/unsupported.
 *
 * @par Confirmed
 * A non-zero first byte of the pad buffer aborts with 0; otherwise the
 * second byte >> 4 is the type, kept only when it equals 4 or 7.
 *
 * @par Likely role
 * The analog-pad detection of the input layer: types 4/7 are the two
 * supported analog controller ids, the menu and camera code branch on
 * them to enable stick input.
 *
 * @par Unknown
 * Which physical pad models ids 4 and 7 denote.
 *
 * @par Callers
 * func_8001E7B8, func_8001EF8C, func_80027510, func_8007C0F8, func_8007C6F0,
 * func_8007C940.
 *
 * @par Role and path
 * Queries the controller type nibble for port in gPadBuffers (D_800A78F0),
 * returning 4 or 7 when unk00[0] is zero and the upper nibble of unk00[1]
 * matches, or 0 otherwise. Reads D_800A78F0 and makes no calls; called by
 * func_8001E7B8, func_8001EF8C, func_80027510, func_8007C0F8, func_8007C6F0,
 * and func_8007C940.
 */
s32 gGetPadType(s32 port)
{
    struct PadBuffer *pad;
    s32 result;

    pad = &gPadBuffers[port];
    if (pad->unk00[0] != 0) {
        result = 0;
    } else {
        result = pad->unk00[1] >> 4;
        switch (result) {
        case 4:
        case 7:
            break;
        default:
            result = 0;
            break;
        }
    }
    return result;
}
