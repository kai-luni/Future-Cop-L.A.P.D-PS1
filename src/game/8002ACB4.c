// Categories: Graphics > Animation | Graphics > Model Renderer > Instance Submission
#include "game/types.h"

struct Unk17508 {
    void *model;
};

union Unk90 {
    u32 word;
    struct {
        u8 pad[2];
        u8 unk92;
        u8 unk93;
    } bytes;
};

struct Unk2ACB4 {
    u8 unk00[0x24];
    u32 flags;
    struct Unk17508 inst;
    u8 unk2C[0x90 - 0x2C];
    union Unk90 u90;
};

extern void func_80013AA0(s32 low, s32 high);
extern s32 gSubmitModelInstance(struct Unk17508 *instance, u32 *flags, s32 arg2) __asm__("func_80017508");

/**
 * @brief Advances one model instance by a frame value with the depth-cue
 *        range saved around the submission.
 *
 * @param object Entity block; +0x24 the flag word, +0x28 the model instance,
 *        +0x90 a flag word whose upper half enables the range save, +0x92/+0x93
 *        the two range bytes.
 * @param value Frame value; only the low byte is forwarded.
 * @return The low byte of the submission result, or 0 without a model.
 *
 * @par Confirmed
 * With a non-null instance model: when the upper half of +0x90 is non-zero,
 * func_80013AA0(+0x92, +0x93) sets the depth-cue range, func_80017508(&inst,
 * &flags, value & 0xFF) submits the instance, and func_80013AA0(0, 0)
 * restores the range. The result is masked to 0xFF.
 *
 * @par Likely role
 * The per-frame animation step of the entity handlers: the message-2 hook
 * func_8002AD58 and the update handlers call it to push the next animation
 * frame into the renderer.
 *
 * @par Callers
 * func_8002AD58, func_8002C3F0, func_8002ED7C, func_800382B4.
 */
s32 func_8002ACB4(struct Unk2ACB4 *object, s32 value)
{
    s32 result;

    if (object->inst.model != 0) {
        register u32 m __asm__("$18");
        m = 0xFFFF0000;

        if ((object->u90.word & m) != 0) {
            func_80013AA0(object->u90.bytes.unk92, object->u90.bytes.unk93);
        }
        result = gSubmitModelInstance(&object->inst, &object->flags, value & 0xFF);
        if ((object->u90.word & m) != 0) {
            func_80013AA0(0, 0);
        }
        return result & 0xFF;
    }
    return 0;
}
