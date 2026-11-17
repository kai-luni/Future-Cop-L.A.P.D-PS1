// Categories: Game Engine > Entity Messages
#include "game/types.h"

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
};

struct EntityMessage {
    u8 kind;
    u8 value;
    s16 timer;
    struct Entity *target;
    s32 id;
};

extern struct EntityMessage D_800A2090[];
extern s32 gPendingMessageMask __asm__("D_8009F7A8");

/**
 *  @brief Allocates and initializes an entity message slot.
 *
 *  Uses an occupancy bitmask to find the first free message entry and
 *  records the target, its current ID, message bytes, and a 600-tick timer.
 *  @param target Entity to associate with the message.
 *  @param kind Message kind byte.
 *  @param value Message value byte.
 *  @return One if a slot was allocated, or zero if the mask is 0xffff.
 *  @par Globals
 *  D_8009F7A8 tracks occupied slots; D_800A2090 stores the messages.
 *  @par Control flow
 *  Rejects a full mask, otherwise scans bits until a free slot is found.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Sets an occupancy bit and writes the selected message entry.
 *  @par Callers
 *  Confirmed: func_8004DA84.
 */
s32 func_8004D9EC(struct Entity *target, u8 kind, u8 value)
{
    struct EntityMessage *message;
    s32 bit;
    s32 result;

    if (gPendingMessageMask == 0xFFFF) {
        return 0;
    }
    message = D_800A2090;
    bit = 1;
    while (gPendingMessageMask & bit) {
        bit <<= 1;
        message++;
    }
    result = 1;
    message->target = target;
    gPendingMessageMask |= bit;
    message->kind = kind;
    message->value = value;
    message->id = target->id;
    message->timer = 600;
    return result;
}
