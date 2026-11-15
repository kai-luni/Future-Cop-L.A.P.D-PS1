// Categories: Graphics > Model Renderer > Vertex Fetch | Graphics > Animation
#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk3D0F0Model {
    u8 unk00[0x0C];
    u32 flags;
    u8 unk10[0x0C];
    u32 unk1C;
    u8 unk20[0x14];
    u32 unk34;
};

struct Unk3D0F0Instance {
    struct Unk3D0F0Model *model;
    u8 unk04[0x30];
    struct Vec3 unk34;
    u8 unk40[0x08];
    s16 frame;
};

extern s32 func_800196C4(void);
extern void func_8003CE30(struct Unk3D0F0Instance *instance, s8 arg1, struct Vec3 *out);

/**
 * @brief Fetches a model point for the current animation frame, optionally
 *        jittered by a random fraction of a per-frame amplitude.
 *
 * @param instance Model instance (frame index, fallback position unk34).
 * @param arg1 Point selector; values 0..3 route to func_8003CE30.
 * @param arg2 When its low byte is nonzero and model flags bit 25 is
 *             clear, the jittered path is taken.
 * @param out Receives the point, each coordinate << 4.
 * @return Nothing.
 *
 * @par Confirmed
 * No model: *out = instance->unk34. Jitter path: value = table[frame + 1]
 * where table = (s32 **)model->unk1C; offset = (((RNG() & 0xFFF) * value)
 * >> 12 << 3) + 4 with func_800196C4 as RNG; the three s16 at that offset
 * are read << 4. Selector < 4: func_8003CE30(instance, arg1, out).
 * Otherwise the point is read from model->unk34 at (frame << 4) + 8.
 *
 * @par Likely role
 * The per-frame amplitude table drives a shake or scatter effect on
 * attached points; bit 25 of the model flags disables it.
 *
 * @par Callers
 * func_8003D240, func_8003DB70, func_80070730, func_80071B40.
 */
void func_8003D0F0(struct Unk3D0F0Instance *instance, s32 arg1, s32 arg2, struct Vec3 *out)
{
    struct Unk3D0F0Model *model;
    u8 *base;
    s32 offset;
    s32 value;
    register s32 frame __asm__("$3");

    model = instance->model;
    if (model != 0) {
        if ((arg2 & 0xFF) != 0) {
            if (((model->flags >> 25) & 1) == 0) {
                value = *((s32 **)model->unk1C)[instance->frame + 1];
                offset = (((u32)((func_800196C4() & 0xFFF) * value) >> 12) << 3) + 4;
                frame = instance->frame;
                __asm__("" : "+r"(frame));
                base = (u8 *)((s32 **)model->unk1C)[frame + 1];
                goto store;
            }
        }

        if (((u32)arg1 & 0xFF) < 4) {
            func_8003CE30(instance, (s8)arg1, out);
            return;
        }

        offset = instance->frame << 4;
        base = (u8 *)instance->model->unk34;
        __asm__("" : "+r"(base), "+r"(offset));
        offset += 8;

    store:
        out->x = ((s16 *)(base + offset))[0] << 4;
        out->y = ((s16 *)(base + offset))[1] << 4;
        out->z = ((s16 *)(base + offset))[2] << 4;
    } else {
        *out = instance->unk34;
    }
}
