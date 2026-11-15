// Categories: Gameplay > Damage > Area | Graphics > Effects
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

extern s32 D_8009F324;
extern u8 D_8009F328;
extern s16 D_8009F710;
extern s16 D_8009F718;
extern s16 D_8009F71C;

extern void func_800410BC(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

/**
 * @brief Tilted area-blast wrapper: publishes the cone geometry globals and
 *        runs the radial blast, then clears the context.
 *
 * @param arg0, arg1, arg2 Forwarded to func_800410BC.
 * @param context Stored in D_8009F324.
 * @param position Cone origin; x/z >> 4 go to D_8009F718/D_8009F71C.
 * @param height >> 4 goes to D_8009F710.
 * @param arg6 Forwarded as the blast's fourth argument.
 * @return Nothing.
 *
 * @par Confirmed
 * Sets D_8009F710/718/71C from height/position, D_8009F328 = 1 (the cone
 * enable), D_8009F324 = context, calls func_800410BC(arg0, arg1, arg2,
 * arg6), then clears D_8009F328 and D_8009F324.
 *
 * @par Likely role
 * The cone/tilted variant of the area damage (sibling of func_800413EC):
 * the three geometry words define the blast cone the radial walker
 * func_800410BC tests against, used by the marker renderer func_8006D520.
 *
 * @par Callers
 * func_8006D520.
 */
void func_80041428(s32 arg0, s32 arg1, s32 arg2, s32 context, struct Vec3s32 *position,
                   s32 height, s32 arg6)
{
    D_8009F710 = height >> 4;
    D_8009F718 = position->x >> 4;
    D_8009F328 = 1;
    D_8009F324 = context;
    D_8009F71C = position->z >> 4;
    func_800410BC(arg0, arg1, arg2, arg6);
    D_8009F328 = 0;
    D_8009F324 = 0;
}
