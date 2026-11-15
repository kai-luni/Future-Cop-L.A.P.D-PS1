// Categories: Graphics > Model Renderer > Skinned Parts | Graphics > GPU Primitives
#include "game/types.h"

extern void func_8001428C(s32 arg0, s32 arg1);
extern void func_80017058(s32 arg0, s32 arg1, s32 arg2);

/**
 * @brief Updates the skinned parts of a model and submits its part
 *        polygons; always reports success.
 *
 * @param arg0 Model instance.
 * @param arg1 Forwarded as the third argument of the draw call.
 * @param arg2 Forwarded to the skinning update.
 * @param arg3 Forwarded as the second argument of the draw call.
 * @return 1.
 *
 * @par Confirmed
 * func_8001428C(arg0, arg2) (skinned parts), then func_80017058(arg0,
 * arg3, arg1) (part submission using the shared depth/tag word
 * D_8009F580).
 *
 * @par Callers
 * func_8003B2D0.
 */
s32 func_800175A0(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    func_8001428C(arg0, arg2);
    func_80017058(arg0, arg3, arg1);
    return 1;
}
