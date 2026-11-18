// Categories: Graphics > Model Renderer > Instance Setup
#include "game/types.h"

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
    u8 unk00[0x0C];
    u32 flags;
    u8 unk10[0x24];
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
 * @brief Initialises a model instance: binds the model, copies the frame-0
 *        bounds and decodes the option flags.
 *
 * @param instance Model instance to initialise.
 * @param model The model (may be 0).
 * @param options Bit field: 0x80000 -> flag 0x10, 0x8000 -> flag 8,
 *        0x10000 -> flag 4.
 * @return Nothing.
 *
 * @par Confirmed
 * With a model: radius = frames[0].radius << 4, unk44/unk46 take the first
 * frame's unk00/unk06; without one all three are 0. frame = 0, flags = 0
 * plus the decoded option bits; model flag bit 30 adds instance flag 0x40.
 *
 * @par Likely role
 * The bind helper behind func_80013BD8: the radius is the culling bound,
 * the two header words are likely the frame count and animation flags,
 * flag 0x40 marks the special model class (bit 30).
 *
 * @par Unknown
 * The exact meaning of unk44/unk46 and option bits beyond the three decoded.
 *
 * @par Callers
 * func_80013BD8, func_8002CB20.
 *
 * @par Environment path
 * Binds a model to a model instance: it stores the model pointer, copies the
 * frame-0 radius shifted left by four together with the two frame header
 * words, resets frame and flags, and decodes three option bits (0x80000,
 * 0x8000, 0x10000) into instance flags, adding 0x40 when bit 30 of the model
 * flag word is set. With a null model the radius and both header words are
 * zeroed. It is instance setup, not a drawing routine: it loads no matrix,
 * emits nothing and touches no global. Its only call sites are func_80013BD8
 * (src/game/80013BD8.c:49) and func_8002CB20 (src/game/8002CB20.c:86), so it
 * sits one level upstream of the render path - the instance it prepares is
 * submitted later by gSubmitModelInstance (func_80017508) through
 * func_8002ACB4.
 */
void func_80013AF0(struct ModelInstance *instance, struct Model *model, u32 options)
{
    instance->model = model;
    if (model == 0) {
        instance->radius = 0;
        instance->unk44 = 0;
        instance->unk46 = 0;
    } else {
        instance->radius = model->frames->frames[0].radius << 4;
        instance->unk44 = instance->model->frames->frames[0].unk00;
        instance->unk46 = instance->model->frames->frames[0].unk06;
    }
    instance->frame = 0;
    instance->flags = 0;
    if (options & 0x80000) {
        instance->flags = 0x10;
    }
    if (options & 0x8000) {
        instance->flags |= 8;
    }
    if (options & 0x10000) {
        instance->flags |= 4;
    }
    if (model != 0 && ((model->flags >> 30) & 1)) {
        instance->flags |= 0x40;
    }
}
