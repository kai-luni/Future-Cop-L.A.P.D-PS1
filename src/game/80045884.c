// Categories: Game Engine > State Tables
#include "game/types.h"

struct Unk45884 {
    s32 position[3];
    s32 angle;
};

extern struct Unk45884 gPositionAngleTable[] __asm__("D_800A1920");

/**
 * @brief Stores a position and adjusted signed-remainder angle in an indexed record.
 *
 *  Copies three position components into the global table and stores the
 *  signed remainder of angle plus 0x1000 divided by 0x1000.
 *  @param position Pointer to three position components.
 *  @param arg1 Unused argument.
 *  @param angle Angle to adjust and reduce modulo 0x1000.
 *  @param index Index of the destination record.
 *  @par Globals
 *  D_800A1920 is the destination table for the position and angle.
 *  @par Control flow
 *  A three-iteration loop copies the position, then the angle is stored.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the selected table record; no index bounds check is performed.
 *  @par Callers
 *  func_80028618 records camera-related position and angle values.
 */
void gSetPositionAngleEntry(s32 *position, s32 arg1, s32 angle, s32 index) __asm__("func_80045884");
void gSetPositionAngleEntry(s32 *position, s32 arg1, s32 angle, s32 index)
{
    s32 i;

    for (i = 0; i < 3; i++) {
        gPositionAngleTable[index].position[i] = position[i];
    }
    angle = (angle + 0x1000) % 0x1000;
    gPositionAngleTable[index].angle = angle;
}
