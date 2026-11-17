// Categories: Gameplay > Damage | Gameplay > Entity Script
#include "game/types.h"

struct Unk80058378DC {
    u8 unk00[0x24];
    u32 unk24;
};

struct Unk80058378AC {
    u8 unk00[0xC1];
    s8 unkC1;
};

struct Unk80058378 {
    u8 unk00[0x1C];
    s16 unk1C;
    u8 unk1E[0x6];
    u32 unk24;
    u8 unk28[0x64];
    s32 unk8C;
    u8 unk90[0x4];
    void *unk94;
    u8 unk98[0x14];
    struct Unk80058378AC *unkAC;
    u8 unkB0[0x2C];
    struct Unk80058378DC *unkDC;
};

struct Unk8009FA94 {
    u8 unk00[0x5C];
    s32 unk5C;
};

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
    u8 unk0C[8];
    u16 unk14;
};

extern s32 gPlayerCount __asm__("D_8009FA0C");
extern struct Unk8009FA94 *D_8009FA94;
extern void *D_8009FA58[2];

extern void func_80058210(void *object, s32 amount);
extern struct Entity *func_8004D664(s32 id);
extern void func_80069D68(void *arg0, s32 arg1, s32 arg2, void *arg3);

/**
 * @brief Applies hit damage to an entity, logs attacker source, runs an optional hit script, and mirrors damage to other players when active.
 *
 * Confirmed: calls func_80058210 to subtract amount from object->unk1C (+0x1C) and award points; if object->unk1C < 0, stores source in object->unk8C (+0x8C). Positive amount sets bit 0x02000000 in object->unk24 and object->unkDC->unk24. When object->unk94 is non-null, looks up (source & 0x3FFFFFFF) via func_8004D664 and calls func_80069D68. Category Gameplay > Damage | Gameplay > Entity Script follows +0x1C/+0x8C damage fields, +0x94 script, and D_8009FA58 player array. Likely: object->unk94 is an on-hit/kill script and D_8009FA58 holds the two player records. Unknown: exact meaning of bit 0x02000000 and D_8009FA94->unk5C == 0.
 *
 * @param object Target entity block (+0x1C health, +0x24 flags, +0x8C source, +0x94 script, +0xAC/+0xDC sub-blocks).
 * @param amount Signed damage amount forwarded to func_80058210.
 * @param source Packed attacker/source identifier stored at +0x8C and masked with 0x3FFFFFFF for lookup.
 *
 * @par Globals
 * D_8009FA0C - player count (>= 2 enables loop). D_8009FA94 - global state (+0x5C == 0 required). D_8009FA58 - 2-element player pointer array.
 *
 * @par Control flow
 * Sequential guards for unk1C < 0, amount > 0, and unk94 != 0, then a 2-entry do-while over D_8009FA58 skipping index == object->unkAC->unkC1.
 *
 * @par Calls
 * func_80058210 - deducts health and scores hit (object and eligible D_8009FA58 entries). func_8004D664 - resolves attacker by id. func_80069D68 - runs unk94 script.
 *
 * @par Side effects
 * Writes object->unk8C, sets bit 0x02000000 on object->unk24 and unkDC->unk24, updates health/score via func_80058210, and runs script via func_80069D68.
 *
 * @par Callers
 * Confirmed: func_80052098 (0x800521C8), func_800584BC (0x8005858C), func_800588D4 (0x80058974), func_8005908C (0x800591C0) / Likely: none / Unknown: none.
 */
void func_80058378(struct Unk80058378 *object, s32 amount, s32 source)
{
    struct Entity *entity;
    s32 i;
    register void **p __asm__("$17");

    func_80058210(object, amount);

    if (object->unk1C < 0) {
        object->unk8C = source;
    }

    if (amount > 0) {
        object->unk24 |= 0x02000000;
        object->unkDC->unk24 |= 0x02000000;
    }

    if (object->unk94 != 0) {
        entity = func_8004D664(source & 0x3FFFFFFF);
        func_80069D68(object->unk94, (u32)object->unk1C >> 31,
                      entity != 0 ? entity->unk14 & 0x3FFF : -1, object);
    }

    if (gPlayerCount >= 2 && D_8009FA94->unk5C == 0) {
        i = 0;
        p = D_8009FA58;
        do {
            if (object->unkAC->unkC1 != i && *p != 0) {
                func_80058210(*p, amount);
            }
            i++;
            p++;
        } while (i < 2);
    }
}
