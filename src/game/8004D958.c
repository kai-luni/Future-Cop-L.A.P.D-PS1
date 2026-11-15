// Categories: Game Engine > Entity Lifecycle | Platform > Sound
#include "game/types.h"

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
    u8 unk0C[0x16];
    s8 sound;
    u8 unk23;
    u32 flags;
    u8 unk28[0x70];
    s32 script;
};

extern s32 func_80069D68(s32 script, s32 arg1, s32 arg2, struct Entity *entity);
extern void func_800457B8(s32 arg0, s32 index);

/**
 * @brief Retires an entity: notifies its script, stops its looping sound
 *        and clears the id.
 *
 * @param entity Entity block; +8 the id, +0x22 the sound channel,
 *        +0x24 flags, +0x98 the script pointer.
 * @return Nothing.
 *
 * @par Confirmed
 * id == 0 aborts (already retired). Without flag 0x8000000 and with a
 * script: func_80069D68(script, ((flags >> 26) ^ 1) & 1, -1, entity) runs
 * the script notification. Flag 0x400000 stops the sound via
 * func_800457B8(id, sound). Finally id = 0 marks the slot free.
 *
 * @par Likely role
 * The universal despawn tail called by the base retire path and the
 * handlers: the id clearing is what the pool GC and the spawn queue's
 * identity checks key on, bit 26 likely flags "no script notify".
 *
 * @par Unknown
 * What the script notification's second argument selects.
 *
 * @par Callers
 * func_8002A810, func_8002FF6C, func_80030060, func_80031120, func_80031C74,
 * func_8003532C, func_80036E6C, func_800398EC, func_8003B144, func_8004CCDC,
 * func_8004CECC, func_8005027C, func_80055988.
 */
void func_8004D958(struct Entity *entity)
{
    if (entity->id == 0) {
        return;
    }
    if (!(entity->flags & 0x8000000) && entity->script != 0) {
        func_80069D68(entity->script, ((entity->flags >> 26) ^ 1) & 1, -1, entity);
    }
    if (entity->flags & 0x400000) {
        func_800457B8(entity->id, entity->sound);
    }
    entity->id = 0;
}
