#include "game/types.h"

struct Vec { s32 x, y, z; };
struct Model { u8 pad[0x34]; char *data; };
struct Extra { u8 pad[0xc1]; s8 index; };
struct Entity {
    u8 pad00[0x14];
    u16 flags;
    u8 type;
    u8 pad17[0x11];
    struct Model *model;
    u8 pad2c[0x24];
    struct Vec position;
    struct Vec other;
    u8 pad68[8];
    s16 part;
    u8 pad72[0x3a];
    struct Extra *extra;
    u8 padb0[4];
    struct Entity *child;
};
struct Owner { u8 pad00[0x94]; void *callback; u8 pad98[8]; struct Vec position; };
extern void func_80010750(void *);
extern void func_80010234(void *, void *, s32);
extern void func_80069D68(void *, s32, s32, void *);
extern s16 D_8009FA44[];

s32 func_80030C60(struct Owner *owner, struct Entity *entity)
{
    int index;
    if (entity->type == 0x12)
        entity = entity->child;
    entity->position = owner->position;
    func_80010750(entity->pad2c);
    func_80010234((entity->part << 4) + 8 + entity->model->data, &entity->other, 1);
    if (entity->type == 1) {
        __asm__ volatile ("" : : : "memory");
        index = entity->extra->index;
        __asm__ volatile ("" : "=r" (index) : "0" (index));
        D_8009FA44[index] = 300;
    }
    if (owner->callback)
        func_80069D68(owner->callback, 0, entity->flags & 0x7fff, owner);
    return 0;
}
