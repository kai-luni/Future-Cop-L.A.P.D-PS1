// Categories: Graphics > Effects > Object Pool | Resources > Memory Management
#include "game/types.h"

struct Unk5A654Sample {
    s16 id;
    u8 rate;
};

struct Unk5A6A4 {
    u8 unk00[0x15];
    u8 slot;
    u8 unk16[0x12];
    s16 scale;
};

extern struct Unk5A654Sample **gTextureDescriptorTable __asm__("D_8009F7D8");

extern s32 gFindTexDescriptorIndex(s32 id) __asm__("func_8005A654");
extern s32 gSpawnDynamicLightNode(struct Unk5A6A4 *object, s32 length) __asm__("func_80049C38");

/**
 * @brief Allocates a pooled effect/sample buffer sized by the table rate and
 *        the object's scale.
 *
 * @param id Pool/table id resolved by func_8005A654.
 * @param object Effect descriptor; +0x15 receives ~index, +0x28 the scale.
 * @param result Optional out pointer for the allocation result.
 * @return The buffer length (rate * scale), or 0 when the id is unknown.
 *
 * @par Confirmed
 * index = func_8005A654(id); a negative index aborts. object->slot = ~index
 * (the complement encodes the slot), length = D_8009F7D8[index]->rate *
 * object->scale, func_80049C38(object, length) performs the allocation and
 * its result is stored through *result when given.
 *
 * @par Likely role
 * The shared allocator of the light/effect spawners (func_800752E0 uses id
 * 0xA for lights): the per-id rate word scales the buffer with the effect
 * size.
 *
 * @par Unknown
 * The exact layout of the D_8009F7D8 table beyond {id, rate}.
 *
 * @par Callers
 * func_80040734, func_800752E0.
 */
s32 func_8005A6A4(s32 id, struct Unk5A6A4 *object, s32 *result)
{
    s32 index;
    s32 length;
    s32 value;

    index = gFindTexDescriptorIndex(id);
    if (index < 0) {
        return 0;
    }
    object->slot = ~index;
    length = gTextureDescriptorTable[index]->rate * object->scale;
    value = gSpawnDynamicLightNode(object, length);
    if (result != 0) {
        *result = value;
    }
    return length;
}
