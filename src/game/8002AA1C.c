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

extern void func_8003BA50(struct Unk3BObject *object, s32 value);
extern void func_80069D68(void *arg0, s32 arg1, u32 arg2,
                          struct Unk3BObject *arg3);

s32 func_8002AA1C(s32 message, struct Unk3BObject *object,
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
                func_8003BA50(object, oldHealth);
            }
        }

        if (object->unk94 != 0) {
            func_80069D68(object->unk94, object->unk1C < 0, source, object);
        }
    }

    return 0;
}
