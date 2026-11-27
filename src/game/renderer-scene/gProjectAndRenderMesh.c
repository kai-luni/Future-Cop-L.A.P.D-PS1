// Categories: Graphics > Model Renderer > Skinned Parts | Graphics > GPU Primitives
#include "game/types.h"

extern void gProjectBoneParts(s32 arg0, s32 arg1) __asm__("func_8001428C");
extern void gDrawModelPartsMasked(s32 arg0, s32 arg1, s32 arg2) __asm__("func_80017058");
s32 gProjectAndRenderMesh(s32 arg0, s32 arg1, s32 arg2, s32 arg3) __asm__("func_800175A0");


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
 *
 * @par Environment path
 * Updates skinned model bone parts via gProjectBoneParts and submits the
 * model's part list for rendering via gDrawModelPartsMasked, func_8001428C,
 * and func_80017058 (called by func_8003B2D0).
 */
s32 gProjectAndRenderMesh(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    gProjectBoneParts(arg0, arg2);
    gDrawModelPartsMasked(arg0, arg3, arg1);
    return 1;
}
