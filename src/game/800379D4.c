// Categories: Gameplay > Collision Response | Gameplay > Entity Update
#include "game/types.h"

typedef struct Obj379D4 {
    u8 pad00[0x1C];
    s16 val1C;
    u8 pad1E[0x24 - 0x1E];
    s32 val24;
    u8 pad28[0x84 - 0x28];
    s32 val84;
    u8 pad88[0xF8 - 0x88];
    s32 posF8;
    s16 posFC;
    u8 padFE[0x10C - 0xFE];
    s16 val10C;
    u8 pad10E[0x118 - 0x10E];
    u32 flags;
    s16 val11C;
    u8 pad11E[2];
    s16 val120;
    u8 pad122[2];
    s16 val124;
} Obj379D4;

extern s32 gEntityBaseHandler(s32 message, Obj379D4 *object, void *data) __asm__("func_8002AD58");
void gResolveRailMoverContact(s32 message, Obj379D4 *object, Obj379D4 *data) __asm__("func_800379D4");


/**
 * @brief Resolves a contact between two objects of the same family.
 *
 * When the object's state bits (0x30) read 1 it is put into state 3 and its
 * countdown is set to -1. The message is forwarded unchanged whenever the
 * object flag 0x8 is set, the partner's counter 0x24 is non-negative, the flag
 * 0x40 is set, either 0x11C is negative, or the two objects do not share the
 * same 0x120 group. For matching groups the object with the larger derived
 * distance (0xFC << 8 - 0xF8) is treated as the faster partner; after the swap
 * the pair must still share the group or be linked through 0x124, otherwise
 * nothing happens. In the update path the faster object takes half of the
 * other's speed, clamped to twice its 0x10C value, the two countdowns are set
 * to -300 and 300 and the partner's speed is halved. Finally flag 0x80 decides
 * whether the base handler sees the message.
 *
 * @param message Entity message id, forwarded to the base handler.
 * @param object Object receiving the contact.
 * @param data Contact partner.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * A chain of goto-driven guard exits around the swap and update blocks.
 *
 * @par Calls
 * func_8002AD58 - base entity handler.
 *
 * @par Side effects
 * Writes state, countdowns and speed fields of both objects.
 *
 * @par Callers
 * Confirmed: func_800382B4. Likely: func_8002C3B0 (same block).
 */
void gResolveRailMoverContact(s32 message, Obj379D4 *object, Obj379D4 *data)
{
    register s32 t2 __asm__("$10");
    register u32 flags __asm__("$9");
    register Obj379D4 *other __asm__("$8");
    register Obj379D4 *temp __asm__("$2");
    register s32 notify __asm__("$2");
    s32 dData;
    s32 dObj;
    s32 half;
    s32 sum;
    s32 range;

    flags = object->flags;
    t2 = message;
    if ((flags >> 30) == 1) {
        object->val1C = -1;
        object->flags |= 0xC0000000;
    }
    other = data;
    if ((flags & 0x8) != 0) {
        goto NOTIFY;
    }
    if (data->val24 >= 0) {
        goto NOTIFY;
    }
    if ((flags & 0x40) != 0) {
        goto COND;
    }
    if (data->val11C < 0) {
        goto COND;
    }
    if (data->val120 == object->val120) {
        dData = (data->posFC << 8) - data->posF8;
        dObj = (object->posFC << 8) - object->posF8;
        if (dData < dObj) {
            goto SWAP;
        }
        goto UPDATE;
    }
    if (data->val124 != object->val120) {
        goto CHECK;
    }
SWAP:
    temp = other;
    other = object;
    object = temp;
CHECK:
    if (other->val120 == object->val120) {
        goto UPDATE;
    }
    if (object->val124 != other->val120) {
        goto COND;
    }
UPDATE:
    half = other->val84 >> 1;
    sum = object->val84 + half;
    range = object->val10C << 1;
    object->val84 = sum;
    if (range < sum) {
        object->val84 = range;
    }
    object->val11C = -300;
    other->val11C = 300;
    other->val84 = other->val84 >> 1;
COND:
    notify = flags & 0x80;
    if (notify != 0) {
      NOTIFY:
        gEntityBaseHandler(t2, object, data);
    }
}
