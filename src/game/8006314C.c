// Categories: Game Engine > Event Queue | Resources > Slot Table
#include "game/types.h"

struct Unk631A0 {
    s32 key;
    u8 value;
};

extern s32 gQueueCount __asm__("D_8009F838");
extern struct Unk631A0 D_800A31A0[];

/**
 * @brief Appends a {key, value} event to the 24-entry frame queue.
 *
 * @param key Event key.
 * @param value Event payload byte.
 * @return 1 when queued, 0 when the queue is full (D_8009F838 == 0x18).
 *
 * @par Confirmed
 * The entry D_800A31A0[D_8009F838] receives value then key, the count
 * increments; at 24 entries the call is dropped.
 *
 * @par Likely role
 * The deferred notification queue of the entity handlers (spawn, hit and
 * state handlers all append here): the consumer drains it once per frame,
 * so events raised mid-tick are processed in order.
 *
 * @par Unknown
 * Which function drains D_800A31A0 and the key vocabulary.
 *
 * @par Callers
 * func_8002C584, func_8002EED8, func_800378BC, func_80041488, func_80041C6C.
 */
s32 func_8006314C(s32 key, u8 value)
{
    struct Unk631A0 *entry;

    if (gQueueCount == 0x18) {
        return 0;
    }
    entry = &D_800A31A0[gQueueCount];
    entry->value = value;
    entry->key = key;
    gQueueCount++;
    return 1;
}
