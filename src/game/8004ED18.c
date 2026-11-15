// Categories: Graphics > Renderer > Entity Pass | Game Engine > Frame
#include "game/types.h"

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
    u8 unk0C[0x17];
    u8 unk23;
    u32 flags;
    s32 unk28;
};

extern struct Entity *D_8009FA40;

extern void func_80065834(struct Entity *entity, s32 arg1, s32 arg2, s32 arg3);

/**
 * @brief Runs one render pass over the entity list, filtered by visibility
 *        flags.
 *
 * @param arg1, arg2, arg3 Forwarded to func_80065834 (the per-entity draw).
 * @return Nothing.
 *
 * @par Confirmed filter
 * For each entity of D_8009FA40: id > 0, byte at +0x23 non-zero, none of the
 * flag bits 0x30 set, and the pointer at +0x28 non-zero.
 *
 * @par Likely role
 * The "draw all visible, active entities" pass; the four conditions are the
 * usual (alive, in view, not suspended, has geometry) test. The excluded bits
 * 0x30 mark entities that another pass already drew (split-screen layers).
 *
 * @par Callers
 * The frame driver of the scene renderer.
 */
void func_8004ED18(s32 arg1, s32 arg2, s32 arg3)
{
    struct Entity *entity;

    for (entity = D_8009FA40; entity != 0; entity = entity->next) {
        if (entity->id > 0 && entity->unk23 != 0 && !(entity->flags & 0x30) &&
            entity->unk28 != 0) {
            func_80065834(entity, arg1, arg2, arg3);
        }
    }
}
