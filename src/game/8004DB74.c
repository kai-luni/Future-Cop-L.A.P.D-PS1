// Categories: Gameplay > Entity Messages
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
extern u32 gPendingMessageMask __asm__("D_8009F7A8");

extern u8 func_8004DA84(struct Entity *entity, u8 kind, u8 value, s32 retry);

/**
 *  @brief Advances timers and attempts delivery of pending entity messages.
 *
 *  Each set bit identifies a message whose timer is reduced by elapsed time.
 *  Expired messages are retried after adding 600 to their timer.
 *  @param elapsed Amount subtracted from each pending message timer.
 *  @par Globals
 *  D_8009F7A8 is the pending-message bitmask and is cleared per completed entry.
 *  D_800A2090 contains the corresponding message records.
 *  @par Control flow
 *  Scans through the highest initially set bit; expired entries with matching
 *  target IDs remain pending only if delivery fails.
 *  @par Calls
 *  func_8004DA84 attempts delivery with retry disabled for matching target IDs.
 *  @par Side effects
 *  Updates timers and the pending mask, and may modify target entities.
 *  @par Callers
 *  Confirmed: func_8002703C.
 */
void func_8004DB74(s32 elapsed)
{
    s32 i;
    u32 pending;
    struct EntityMessage *message;
    u32 bit;

    if (gPendingMessageMask == 0) {
        return;
    }
    pending = gPendingMessageMask;
    message = D_800A2090;
    i = 0;
    bit = 1;
    do {
        if (!(pending & 1)) {
            goto next;
        }
        message->timer -= elapsed;
        if (message->timer > 0) {
            goto next;
        }
        message->timer += 600;
        if (message->target->id == message->id &&
            func_8004DA84(message->target, message->kind, message->value, 0) == 0) {
            goto next;
        }
        gPendingMessageMask ^= bit << i;
next:
        i++;
        message++;
        pending >>= 1;
    } while (pending != 0);
}
