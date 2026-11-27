// Categories: Gameplay > Entity Messages
#include "game/types.h"

struct EntitySlot {
    u8 kind;
    u8 value;
};

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
    struct EntitySlot slots[4];
    u8 unk14;
    u8 unk15;
    u8 type;
};

extern u8 gIsQueuedCommandKind(u32 value) __asm__("func_8004DA58");
extern u8 gEnqueuePendingMessage(struct Entity *target, s32 kind, s32 value) __asm__("func_8004D9EC");
u8 gSendEntityCommand(struct Entity *entity, s32 kind, s32 value, s32 retry) __asm__("func_8004DA84");


/**
 *  @brief Accepts or queues a kind and value for an entity.
 *
 *  Certain entity types report success without storing the pair; other types
 *  use the first of four slots whose kind is zero.
 *  @param entity Entity to receive or accept the pair.
 *  @param kind Kind to filter and store in a slot.
 *  @param value Value to store alongside the kind.
 *  @param retry Nonzero to invoke the fallback when all slots are occupied.
 *  @return One for immediate acceptance or slot storage, zero for failure, or the fallback result.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  Dispatches by entity type, scans up to four slots, then optionally falls back.
 *  @par Calls
 *  func_8004DA58 filters kinds for types 18 and 21; func_8004D9EC is the fallback.
 *  @par Side effects
 *  May write a slot's byte-sized kind and value or invoke the fallback.
 *  @par Callers
 *  Confirmed: func_8004DC64, func_8004DD68, func_8004DDF4, func_80069D68.
 */
u8 gSendEntityCommand(struct Entity *entity, s32 kind, s32 value, s32 retry)
{
    s32 i;

    switch (entity->type) {
    case 18:
    case 21:
        if (gIsQueuedCommandKind(kind) != 0) {
            break;
        }
        /* Failed filter joins the immediate-success return. */
    case 2:
    case 3:
    case 4:
    case 7:
    case 15:
    case 23:
        return 1;
    case 5:
    case 6:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 16:
    case 17:
    case 19:
    case 20:
    case 22:
    default:
        break;
    }

    for (i = 0; i < 4; i++) {
        if (entity->slots[i].kind == 0) {
            entity->slots[i].kind = kind;
            entity->slots[i].value = value;
            return 1;
        }
    }

    if (retry != 0) {
        return gEnqueuePendingMessage(entity, kind, value);
    }
    return 0;
}
