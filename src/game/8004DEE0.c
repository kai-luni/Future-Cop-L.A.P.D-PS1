// Categories: Gameplay > Entity Script
#include "game/types.h"

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
    u8 unk0C[0x10];
    s16 unk1C;
    u8 unk1E[0x7E];
    u8 *script;
};

extern s32 func_80069D68(u8 *script, s32 arg1, s32 arg2, struct Entity *entity);
void gRunEntityScript(struct Entity *entity) __asm__("func_8004DEE0");


/**
 * @brief Runs the entity script with the current death state.
 *
 * Only entities that still carry a script opcode stream behave: the first byte
 * must be non-zero. The script gets the dead/alive flag from the health
 * countdown 0x1C (< 0 means dead) and -1 as its argument, with the entity as
 * context - the same call shape the damage and timer paths use.
 * @param entity Entity with script pointer at 0xD2 and health at 0x1C.
 * @par Globals
 * None.
 * @par Control flow
 * One guard, then the VM call.
 * @par Calls
 * func_80069D68 - script VM.
 * @par Side effects
 * Whatever the script does to the entity.
 * @par Callers
 * Unknown: no direct edge; called from the entity update loop.
 */
void gRunEntityScript(struct Entity *entity)
{
    if (entity->script != 0 && entity->script[0] != 0) {
        func_80069D68(entity->script, entity->unk1C < 0, -1, entity);
    }
}
