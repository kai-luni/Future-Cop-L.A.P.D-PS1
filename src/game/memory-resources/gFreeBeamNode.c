// Categories: Resources > Memory Management | Graphics > Effects
#include "game/types.h"

struct Unk71A68Entry {
    void *data;
    void *extra;
    s32 unk08;
    s32 unk0C;
};

struct Unk71A68 {
    s32 unk00;
    void *first;
    void *second;
    s16 count;
    s16 unk0E;
    struct Unk71A68Entry *entries;
};

extern void gFreeBeamEndpointBlock(void *block) __asm__("func_80070710");
extern void gFreeBlock(void *block) __asm__("func_800181D4");
void gFreeBeamNode(struct Unk71A68 *object) __asm__("func_80071A68");


/**
 * @brief Destroys a compound effect object: two pooled blocks, an entry
 *        array with per-entry extras, and the object itself.
 *
 * @param object Effect block; +4/+8 two pooled pointers, +0xC the s16 entry
 *        count, +0x10 the entry array (each entry: data, extra, two words).
 * @return Nothing.
 *
 * @par Confirmed
 * first/second are released via func_80070710 when non-null; every entry's
 * extra (when set) and data go to func_800181D4, then the entry array and
 * the object itself are freed.
 *
 * @par Likely role
 * The cleanup case of the effect handler func_8007364C: the object owns a
 * small tree of pool allocations that must all return together.
 *
 * @par Callers
 * func_8007364C.
 */
void gFreeBeamNode(struct Unk71A68 *object)
{
    s32 i;

    if (object->first != 0) {
        gFreeBeamEndpointBlock(object->first);
    }
    if (object->second != 0) {
        gFreeBeamEndpointBlock(object->second);
    }
    for (i = 0; i < object->count; i++) {
        if (object->entries[i].extra != 0) {
            gFreeBlock(object->entries[i].extra);
        }
        gFreeBlock(object->entries[i].data);
    }
    gFreeBlock(object->entries);
    gFreeBlock(object);
}
