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
s32 gSpawnDynamicLightEffect(s32 id, struct Unk5A6A4 *object, s32 *result) __asm__("func_8005A6A4");


/**
 * @brief Resolves a descriptor id, records its complemented slot, and spawns a
 *        dynamic light node with length rate * scale.
 *
 * @param id Pool/table id resolved by func_8005A654.
 * @param object Effect descriptor; +0x15 receives ~index, +0x28 the scale.
 * @param result Optional out pointer receiving the func_80049C38 return value.
 * @return The scaled length (rate * scale), or 0 when the id is unknown.
 *
 * @par Confirmed
 * index = func_8005A654(id); a negative index aborts. object->slot = ~index
 * (the complement encodes the slot), length = D_8009F7D8[index]->rate *
 * object->scale, func_80049C38(object, length) spawns the light node and
 * its result is stored through *result when given.
 *
 * @par Likely role
 * Shared dynamic-light effect launcher (func_800752E0 uses id 0xA for lights):
 * the per-id rate byte scales the duration/length with the object's scale.
 *
 * @par Unknown
 * The exact layout of the D_8009F7D8 table beyond {id, rate}.
 *
 * @par Callers
 * func_80040734, func_800752E0.
 */
s32 gSpawnDynamicLightEffect(s32 id, struct Unk5A6A4 *object, s32 *result)
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
