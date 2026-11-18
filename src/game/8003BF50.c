// Categories: Gameplay > Object Classification
#include "game/types.h"

extern int gWriteCellEventValue();
extern int gAttachPartEffect();

struct Unk3BF50Object {
    u8 unk00[0x16];
    u8 kind;
    u8 unk17[0xA9];
    struct Unk3BF50Object *unkC0;
    u8 unkC4[4];
    struct Unk3BF50Object *unkC8;
};

/**
 * @brief Tests whether an object is, or is linked to, a kind-0x12 entity.
 *
 * @param object Entity block; +0x16 the kind byte, +0xC0 and +0xC8 two
 *        linked object pointers.
 * @return 1 for kind 1 or 0x12 directly; for kind 7 the test follows +0xC0,
 *         for kind 2 or 0x0F it follows +0xC8, and returns 1 only when that
 *         linked object's kind is 0x12; 0 otherwise (also for a null object).
 *
 * @par Confirmed
 * Pure switch over the kind byte with the two link follow-ups; no writes.
 *
 * @par Likely role
 * The "is this the player (or attached to the player)" predicate of the
 * entity handlers: kind 0x12 marks the player class, kinds 7/2/0x0F are
 * carried or linked objects that inherit the answer from their partner.
 *
 * @par Unknown
 * The exact meaning of kinds 1, 2, 7 and 0x0F beyond the link direction.
 *
 * @par Callers
 * func_8003B898, func_8003BAE4, func_8003BC0C, func_8005B6A8, func_8005BDE4,
 * gWriteCellEventValue, func_8005C184, func_8005C344, gAttachPartEffect.
 */
s32 func_8003BF50(struct Unk3BF50Object *object)
{
    struct Unk3BF50Object *other;
    s32 result;

    if (object != 0) {
        switch (object->kind) {
        case 1:
        case 0x12:
            return 1;
        case 7:
            other = object->unkC0;
            result = 0;
            goto check;
        case 2:
        case 0xF:
            other = object->unkC8;
            result = 0;
        check:
            if (other != 0) {
                result = other->kind == 0x12;
            }
            return result;
        default:
            return 0;
        }
    }
    return 0;
}
