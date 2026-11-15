// Categories: Graphics > Camera > Rotation
#include "game/types.h"

struct Camera {
    u8 unk00[0x18];
    s32 roll;
    s32 pitch;
    s32 yaw;
    u8 unk24[4];
    u8 matrix[0x30];
    u8 rotation[0x20];
};

extern struct Camera *D_8009F9E4;

extern void func_800188D0(s32 yaw, s32 pitch, s32 roll, u8 *rotation);
extern void func_80018860(u8 *rotation, u8 *matrix);

/**
 * @brief Sets the active camera's rotation from yaw/pitch/roll and rebuilds
 *        its matrix.
 *
 * @param yaw, pitch, roll The three angles (12-bit units).
 * @return Nothing.
 *
 * @par Confirmed
 * func_800188D0(yaw, pitch, roll, camera->rotation) builds the rotation
 * (the shared rotation factory), func_80018860 converts it into the
 * camera matrix at +0x2C; the three angles are stored at +0x18/+0x1C/+0x20
 * of D_8009F9E4.
 *
 * @par Likely role
 * The camera orientation setter of the view placement code (func_80056634
 * and the camera handlers): every view change routes through the same
 * rotation factory so software and GTE paths stay consistent.
 *
 * @par Callers
 * func_80056634, func_8005678C, func_800569EC, func_800573A4.
 */
void func_80017860(s32 yaw, s32 pitch, s32 roll)
{
    func_800188D0(yaw, pitch, roll, D_8009F9E4->rotation);
    func_80018860(D_8009F9E4->rotation, D_8009F9E4->matrix);
    D_8009F9E4->roll = roll;
    D_8009F9E4->pitch = pitch;
    D_8009F9E4->yaw = yaw;
}
