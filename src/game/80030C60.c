// Categories: Gameplay > Entity Draw | Graphics > Model Renderer
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
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *, void *, s32) __asm__("func_80010234");
extern void func_80069D68(void *, s32, s32, void *);
extern s16 gPlayerFadeTimers[] __asm__("D_8009FA44");
s32 gWarpEntityToSelf(struct Owner *owner, struct Entity *entity) __asm__("func_80030C60");


/**
 * @brief Draws one vertex of an entity and hands it to the entity callback.
 *
 * @param owner Owner block: callback at +0x94, position at +0xA0.
 * @param entity Entity: type at +0x16, flags at +0x14, model at +0x28,
 *        transform at +0x2C, position at +0x50, part index at +0x70.
 * @return 0 on the paths that reach the end of the function.
 *
 * @par Confirmed
 * A type of 0x12 makes the function continue with entity->child instead. The
 * matrix at +0x2C is loaded with func_80010750, one vertex is transformed
 * (func_80010234 with source `model->data + (part << 4) + 8`), and
 * func_80069D68(owner->callback, 0, entity->flags & 0x7FFF, owner) is called when
 * the owner has a callback.
 *
 * @par Likely role
 * The per-entity "attach this part's screen position to the caller" step of the
 * model pass; 0x12 is the two-part entity type.
 *
 * @par Environment path
 * Message-11 body of the type-29 handler func_80030D3C: it moves the entity
 * that sent the message (or that entity's child when its type is 0x12) to
 * this entity's position at +0xA4, loads its matrix with func_80010750 and
 * transforms the part selected by +0x70 with func_80010234. When the moved
 * entity is type 1 it sets the screen-fade timer D_8009FA44 for the player
 * index at +0xC1 of its extra record to 300, and it runs this entity's script
 * with func_80069D68 when one is attached. It always returns 0.
 */
s32 gWarpEntityToSelf(struct Owner *owner, struct Entity *entity)
{
    int index;
    if (entity->type == 0x12)
        entity = entity->child;
    entity->position = owner->position;
    gGteLoadMatrix(entity->pad2c);
    gGteTransformVerticesWide((entity->part << 4) + 8 + entity->model->data, &entity->other, 1);
    if (entity->type == 1) {
        __asm__ volatile ("" : : : "memory");
        index = entity->extra->index;
        __asm__ volatile ("" : "=r" (index) : "0" (index));
        gPlayerFadeTimers[index] = 300;
    }
    if (owner->callback)
        func_80069D68(owner->callback, 0, entity->flags & 0x7fff, owner);
    return 0;
}
