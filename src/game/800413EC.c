// Categories: Gameplay > Damage > Area | Graphics > Effects
#include "game/types.h"

extern s32 gMessageConeFlag __asm__("D_8009F324");
extern u8 gBandTestFlag __asm__("D_8009F329");

extern void func_800410BC(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

/**
 * @brief Flat area-blast wrapper: sets the context globals and runs the
 *        radial blast, then clears them.
 *
 * @param arg0, arg1, arg2 Forwarded to func_800410BC (position/extent).
 * @param context Stored in D_8009F324 (the blast's source object).
 * @param arg4 Forwarded as the blast's fourth argument.
 * @param mode ORed with 1 into D_8009F329 (the band bits).
 * @return Nothing.
 *
 * @par Confirmed
 * D_8009F324 = context, D_8009F329 = mode | 1, func_800410BC(arg0, arg1,
 * arg2, arg4), then both globals reset to 0. The globals are the shared
 * blast parameter block the radial walker reads.
 *
 * @par Likely role
 * The flat (ground-plane) variant of the area damage used by the marker
 * renderer func_8006D520; bit 0 of the band byte likely marks "active".
 *
 * @par Callers
 * func_8006D520.
 */
void func_800413EC(s32 arg0, s32 arg1, s32 arg2, s32 context, s32 arg4, s32 mode)
{
    gMessageConeFlag = context;
    gBandTestFlag = mode | 1;
    func_800410BC(arg0, arg1, arg2, arg4);
    gBandTestFlag = 0;
    gMessageConeFlag = 0;
}
