// Categories: Graphics > Model Renderer > Instance Submission | Graphics > Culling
#include "game/types.h"

struct Unk17508 {
    void *model;
};

extern u8 func_80013F08(struct Unk17508 *instance);
extern void func_80016A64(struct Unk17508 *instance, s32 mode, s32 arg2);

/**
 * @brief Visibility gate and submission for one model instance.
 *
 * @param instance Object whose first field is the model pointer.
 * @param flags Draw flags; bit 0x20 suppresses drawing, bit 25 selects the
 *              draw mode passed on.
 * @param arg2 Forwarded to the draw call unchanged.
 * @return 0 when skipped or culled, otherwise the visibility result (u8).
 *
 * @par Confirmed
 * Returns 0 when instance->model is null or *flags & 0x20. Otherwise
 * visible = func_80013F08(instance) (instance culling/submission test); if
 * visible, func_80016A64(instance, (*flags >> 25) & 1, arg2) draws it.
 *
 * @par Callers
 * func_8002ACB4, func_80030960, func_8003CF6C, func_800597C0.
 */
u8 func_80017508(struct Unk17508 *instance, u32 *flags, s32 arg2)
{
    u8 visible;

    if (instance->model == 0 || (*flags & 0x20)) {
        return 0;
    }
    visible = func_80013F08(instance);
    if (visible) {
        func_80016A64(instance, (*flags >> 25) & 1, arg2);
    }
    return visible;
}
