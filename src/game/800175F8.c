// Categories: Graphics > Animation > Frame Control
#include "game/types.h"

extern int gGuidedProjectileUpdate();

struct ModelFrame {
    u16 unk00;
    s16 unk02;
    s16 radius;
    u16 unk06;
    u8 unk08[8];
};

struct ModelFrames {
    u8 header[0x10];
    struct ModelFrame frames[1];
};

struct Model {
    u8 unk00[0x34];
    struct ModelFrames *frames;
};

struct ModelInstance {
    struct Model *model;
    u8 coord[0x3C];
    s32 radius;
    u16 unk44;
    u16 unk46;
    s16 frame;
    u8 flags;
};

/**
 * @brief Sets the model instance's frame and refreshes the per-frame
 *        bounds and header words.
 *
 * @param instance Model instance.
 * @param frame New frame index.
 * @return Nothing.
 *
 * @par Confirmed
 * A no-op when the frame is unchanged; otherwise frame is stored and
 * radius = frames[frame].radius << 4, unk44/unk46 = the frame's
 * unk00/unk06 (the same triple func_80013AF0 initialises from frame 0).
 *
 * @par Likely role
 * The animation tick's frame commit: every handler that advances an
 * animation calls this so the culling radius follows the pose.
 *
 * @par Callers
 * func_8002B588, func_8002B784, func_8002CC30, func_8002DB9C, func_8002DEA8,
 * func_8002E630, func_800306A0, func_800324E0, gGuidedProjectileUpdate, func_800382B4,
 * func_80038A00, func_80051AF8, func_800549F0, func_80055988.
 */
void func_800175F8(struct ModelInstance *instance, s32 frame)
{
    if (instance->frame == frame) {
        return;
    }
    instance->frame = frame;
    instance->radius = instance->model->frames->frames[frame].radius << 4;
    instance->unk44 = instance->model->frames->frames[frame].unk00;
    instance->unk46 = instance->model->frames->frames[frame].unk06;
}
