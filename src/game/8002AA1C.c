// Categories: Gameplay > Damage | Gameplay > Health
#include "game/types.h"

struct Unk2AA1CAttacker {
    u8 unk00[8];
    s32 id;
};

struct Unk2AA1CHit {
    u32 unk00;
    s16 damage;
    u8 kind;
    u8 unk07;
    union {
        struct Unk2AA1CAttacker *attacker;
        u32 source;
    } from;
    u8 unk0C[8];
    u16 unk14;
    u8 unk16[4];
    s16 amount;
};

struct Unk3BObject {
    u8 unk00[0x19];
    u8 flags19;
    u8 unk1A[2];
    s16 unk1C;
    s16 unk1E;
    u8 tableIndex;
    u8 unk21[3];
    u32 flags;
    u8 unk28[0x64];
    u32 unk8C;
    u8 unk90[4];
    void *unk94;
};

extern void gFlagQuartileChange(struct Unk3BObject *object, s32 value) __asm__("func_8003BA50");
extern void func_80069D68(void *arg0, s32 arg1, u32 arg2,
                          struct Unk3BObject *arg3);
s32 gApplyEntityBaseDamage(s32 message, struct Unk3BObject *object,
                  struct Unk2AA1CHit *hit) __asm__("func_8002AA1C");


/**
 * @brief Applies a hit to an entity and runs its damage script.
 *
 * Entities with flag 0x80 ignore hits completely. Message 0xE takes the damage
 * from the hit record and the source word from its 0x00 field, every other
 * message takes them from 0x1A and 0x14. A positive amount sets flag
 * 0x02000000, saves the old health from 0x1C and subtracts the amount. Message
 * 0xE additionally packs the hit kind into 0x19 (kind 2 and above), other
 * messages reset the low nibble of that byte. When the health goes negative the
 * attacker id (message 0xE) or the raw source is stored in 0x8C; otherwise the
 * health bar is notified through func_8003BA50. The script attached at 0x94 is
 * always run with the "dead" boolean and the source as context.
 *
 * @param message Message id, 0xE means a direct hit.
 * @param object Entity that takes the damage.
 * @param hit Hit record with amount, kind and source/attacker.
 * @return Always 0.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Guard, then two parallel layouts for the damage source, then the health test.
 *
 * @par Calls
 * func_8003BA50 - health bar update. func_80069D68 - damage script.
 *
 * @par Side effects
 * Writes health, flags and the kill-credit word.
 *
 * @par Callers
 * Confirmed: func_8002AD58 (base entity handler, message 0xE).
 */
s32 gApplyEntityBaseDamage(s32 message, struct Unk3BObject *object,
                  struct Unk2AA1CHit *hit)
{
    s32 amount;
    u32 source;
    s32 kind;
    s32 oldHealth;

    if (object->flags & 0x80) {
        return 0;
    }

    if (message == 0xE) {
        amount = hit->damage;
        source = hit->unk00;
    } else {
        amount = hit->amount;
        source = hit->unk14;
    }
    source &= 0x7FFF;

    if (amount > 0) {
        object->flags |= 0x02000000;
        oldHealth = object->unk1C;
        object->unk1C -= amount;

        if (message == 0xE) {
            kind = hit->kind;
            if (kind >= 2) {
                object->flags19 |= (kind << 3) & 0x7F0;
            }
        } else {
            object->flags19 &= 0xF;
        }

        if (object->unk1C < 0) {
            if (message == 0xE) {
                if (hit->from.attacker != 0) {
                    object->unk8C = hit->from.attacker->id | 0x40000000;
                }
            } else {
                object->unk8C = hit->from.source;
            }
        } else {
            if (object->tableIndex != 0 && (object->flags & 0x40000) == 0) {
                gFlagQuartileChange(object, oldHealth);
            }
        }

        if (object->unk94 != 0) {
            func_80069D68(object->unk94, object->unk1C < 0, source, object);
        }
    }

    return 0;
}
