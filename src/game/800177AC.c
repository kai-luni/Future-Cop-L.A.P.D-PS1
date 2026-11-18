// Categories: Graphics > Camera > Position
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Camera {
    struct Vec3s32 position;
    u8 unk0C[0x1C];
    u8 matrix[0x24];
    struct Vec3s32 inverse;
    u8 unk58[0x24];
    struct Vec3s32 eye;
};

extern struct Camera *gCamera __asm__("D_8009F9E4");

extern void gMulMatrixVector(u8 *matrix, struct Vec3s32 *input, struct Vec3s32 *output) __asm__("func_8001A81C");
void gSetCameraPosition(struct Vec3s32 *position) __asm__("func_800177AC");


/**
 * @brief Sets the active camera's position and recomputes the inverse
 *        translation.
 *
 * @param position The new camera position triple.
 * @return Nothing.
 *
 * @par Confirmed
 * D_8009F9E4->position and ->eye take the value; func_8001A81C rotates the
 * position by the camera matrix, the negated result becomes ->inverse (the
 * view translation the GTE path uses).
 *
 * @par Likely role
 * The camera move setter paired with func_80017860 (rotation): the view
 * placement code calls both to position and orient the active view.
 *
 * @par Callers
 * func_80056634, func_8005678C, func_800569EC, func_800573A4.
 *
 * @par Environment path
 * Minimal-environment step 1: writes the eye position of a view record and
 * its inverse. The four view placements (func_80056634, func_8005678C,
 * func_800569EC, func_800573A4) call it together with func_80017860.
 */
void gSetCameraPosition(struct Vec3s32 *position)
{
    struct Vec3s32 rotated;

    gCamera->position = *position;
    gCamera->eye = *position;
    gMulMatrixVector(gCamera->matrix, position, &rotated);
    rotated.x = -rotated.x;
    rotated.y = -rotated.y;
    rotated.z = -rotated.z;
    gCamera->inverse = rotated;
}
