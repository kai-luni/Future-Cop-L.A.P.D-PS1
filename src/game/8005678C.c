// Categories: Graphics > Camera > Placement
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk5678COffset {
    s16 angle;
    s16 pitch;
    s32 distance;
    u8 data[0x0C];
};

struct Unk5678CInfo {
    u8 unk00[0xA8];
    s16 unkA8;
    u8 unkAA[0x12];
    s32 unkBC;
};

struct Unk5678C {
    u8 unk00[0x5C];
    struct Vec3s32 position;
    u8 unk68[0x44];
    struct Unk5678CInfo *info;
    s16 heading;
};

extern struct Unk5678COffset gViewPresetTable[] __asm__("D_8009AD08");

extern void gComputeCameraEye(struct Vec3s32 *position, s32 heading, s32 angle, s32 pitch,
                          s32 distance, u8 *data, s32 *out, s32 arg7, s32 arg8) __asm__("func_8005612C");
extern void gSetCameraRotation(s32 angle, s32 pitch, s32 arg2) __asm__("func_80017860");
extern void gSetCameraPosition(s32 *out) __asm__("func_800177AC");
void gPlaceCameraFromPreset(struct Unk5678C *object, s32 index, s32 *out, s32 apply) __asm__("func_8005678C");


/**
 * @brief Places the camera from a preset offset around an object, optionally
 *        applying the result to the active view.
 *
 * @param object Follow target; +0x5C the position, +0xB0 the info block,
 *        +0xB4 the heading.
 * @param index Preset index into D_8009AD08 (angle/pitch/distance/data).
 * @param out Receives the computed camera position.
 * @param apply Non-zero also sets the active camera rotation and position.
 * @return Nothing.
 *
 * @par Confirmed
 * func_8005612C computes the offset position from the object position and
 * heading (heading + preset angle, preset pitch/distance, the info words
 * +0xA8/+0xBC). With apply: func_80017860(heading + angle, pitch, 0) sets
 * the rotation and func_800177AC(out) the position of D_8009F9E4.
 *
 * @par Likely role
 * The preset-driven camera placement of the chase rig (func_800569EC) and
 * func_80056870: the preset table is the same D_8009AD08 the view handlers
 * cycle through.
 *
 * @par Callers
 * func_80056870, func_800569EC.
 *
 * @par Environment path
 * Computes a camera position from a preset offset around a follow target and,
 * when asked, applies it through gSetCameraPosition/gSetCameraRotation.
 */
void gPlaceCameraFromPreset(struct Unk5678C *object, s32 index, s32 *out, s32 apply)
{
    struct Vec3s32 position;
    struct Unk5678COffset *offset;
    s32 heading;

    heading = object->heading;
    position = object->position;
    offset = &gViewPresetTable[index];
    gComputeCameraEye(&position, heading, heading + offset->angle, offset->pitch,
                  offset->distance, offset->data, out, object->info->unkA8,
                  object->info->unkBC);
    if (apply) {
        gSetCameraRotation(object->heading + offset->angle, offset->pitch, 0);
        gSetCameraPosition(out);
    }
}
