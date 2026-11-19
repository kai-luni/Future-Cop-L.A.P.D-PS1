// Categories: Gameplay > Damage | Gameplay > Entity Script
#include "game/types.h"

struct Unk584BCAttacker {
    u8 unk00[8];
    s32 id;
};

struct Unk584BCHit {
    u8 unk00[4];
    s16 damage;
    u8 kind;
    u8 unk07;
    union {
        struct Unk584BCAttacker *attacker;
        s32 source;
    } from;
    u8 unk0C[0x0E];
    s16 amount;
    u8 unk1C[8];
    u32 flags;
};

struct Unk584BCInfo {
    u8 unk00[0x31A];
    s16 unk31A;
};

struct Unk584BC {
    u8 unk00[0x19];
    u8 flags19;
    u8 unk1A[0x92];
    struct Unk584BCInfo *info;
};

extern s32 gSpawnTypeAtOwner(struct Unk584BC *owner, u8 column, s32 arg2, s32 row, s32 arg4) __asm__("func_800402DC");
extern void func_80058378(struct Unk584BC *object, s32 amount, s32 source);

/**
 * @brief Processes incoming hit/damage messages (11 and 14) for a unit entity.
 *
 * Extracts damage amount and attacker/source id from hit according to message (0x0E direct hit vs other hit messages), applies kind-0x10 timer/effect handling, merges hit kind bits into object->flags19 (+0x19), and forwards positive damage to func_80058378.
 *
 * @param message Handler message id (0x0E reads damage/+0x04 and attacker->id | 0x40000000; others check +0x24 bit 0x40 and read amount/+0x1A and source/+0x08).
 * @param object Target entity (+0x19 flags byte, +0xAC info sub-block with +0x31A timer).
 * @param hit Hit/damage payload record.
 * @return Always 0.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Branches on message == 0x0E: builds source from attacker->id | 0x40000000 if non-null, reads hit->damage, and when hit->kind == 0x10 sets object->info->unk31A = 600 and calls func_800402DC. Otherwise returns 0 if hit->flags & 0x40, else reads hit->amount and hit->from.source. For amount > 0 and kind >= 2, ORs (kind << 3) & 0x7F0 into u8 object->flags19 (high nibble stored) and calls func_80058378.
 *
 * @par Calls
 * func_800402DC (spawns column-0xCD row-1 effect on message 0x0E and kind 0x10); func_80058378 (applies positive damage and source to object).
 *
 * @par Side effects
 * May set object->info->unk31A = 600, update object->flags19 (+0x19), spawn an effect, and apply damage via func_80058378.
 *
 * @par Callers
 * Confirmed: func_800588D4 (0x80058B58, message 0x0E), func_8005908C (0x800591D8, messages 11/14), and func_80059ABC (0x80059B5C, forwarding child messages 11/14).
 */
s32 func_800584BC(s32 message, struct Unk584BC *object, struct Unk584BCHit *hit)
{
    s32 amount;
    s32 source;
    s32 kind;

    if (message == 0xE) {
        source = 0;
        if (hit->from.attacker != 0) {
            source = hit->from.attacker->id | 0x40000000;
        }
        amount = hit->damage;
        if (hit->kind == 0x10) {
            object->info->unk31A = 600;
            gSpawnTypeAtOwner(object, 0xCD, 0, 1, 0);
        }
    } else {
        if (hit->flags & 0x40) {
            return 0;
        }
        amount = hit->amount;
        source = hit->from.source;
    }
    if (amount > 0) {
        kind = hit->kind;
        if (kind >= 2) {
            object->flags19 |= (kind << 3) & 0x7F0;
        }
        func_80058378(object, amount, source);
    }
    return 0;
}
