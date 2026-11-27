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

extern struct Camera *gCamera __asm__("D_8009F9E4");

extern void gBuildRotationMatrix(s32 yaw, s32 pitch, s32 roll, u8 *rotation) __asm__("func_800188D0");
extern void gBuildViewMatrix(u8 *rotation, u8 *matrix) __asm__("func_80018860");
void gSetCameraRotation(s32 yaw, s32 pitch, s32 roll) __asm__("func_80017860");


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
 *
 * @par Environment path
 * Minimal-environment step 2: turns yaw/pitch/roll of a view record into its
 * 3x3 matrix and the uploadable view matrix (via func_80018860). The four
 * view placements call it together with func_800177AC.
 */
void gSetCameraRotation(s32 yaw, s32 pitch, s32 roll)
{
    gBuildRotationMatrix(yaw, pitch, roll, gCamera->rotation);
    gBuildViewMatrix(gCamera->rotation, gCamera->matrix);
    gCamera->roll = roll;
    gCamera->pitch = pitch;
    gCamera->yaw = yaw;
}
