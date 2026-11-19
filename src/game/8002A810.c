// Categories: Game Engine > Entity Lifecycle | Gameplay > Map > Occupancy
#include "game/types.h"

typedef struct Unk8002A810 Unk8002A810;
typedef struct Unk8002A810Sub Unk8002A810Sub;
typedef struct Unk8002A810Local Unk8002A810Local;

struct Unk8002A810Sub {
    u8 unk00[0x16];
    u8 unk16;
    u8 unk17[0x03];
    u16 unk1A;
    u8 unk1C[0x5C];
    s32 unk78;
    s32 unk7C;
    s32 unk80;
};

struct Unk8002A810Local {
    u8 unk00[4];
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    u8 unk10[0x30];
    u16 unk40;
    u8 unk42[7];
    u8 unk49;
    u8 unk4A[6];
};

struct Unk8002A810 {
    u8 unk00[0x19];
    u8 unk19;
    u8 unk1A[0x06];
    u8 unk20;
    u8 unk21[0x03];
    u32 unk24;
    u8 unk28[0x4C];
    void *unk74;
    u8 unk78[0x14];
    u32 unk8C;
};

extern Unk8002A810Sub *func_8004D664(s32 id);
extern void gSpawnEntitySlotTypes(s32 arg0, Unk8002A810 *arg1,
                         Unk8002A810Local *arg2, s32 arg3, s32 arg4) __asm__("func_8003B898");
extern void func_80031690(Unk8002A810 *arg0);
extern void func_8004D958(Unk8002A810 *arg0);
s32 gRetireEntityBase(Unk8002A810 *s0, s32 arg1, s32 arg2) __asm__("func_8002A810");


/**
 * @brief The base retire implementation (message 4): marks the entity
 *        retired, clears its map footprint and runs the despawn tail.
 *
 * @param s0 Entity block; +0x24 flags, +0x20 the map region,
 *        +0x74 an optional hook block, +0x8C the owner id.
 * @param arg1 Non-zero runs func_8004D958 (script/sound/id retire).
 * @param arg2 Non-zero refreshes the map occupancy.
 * @return 1 when processed, 0 when already retired (flag 0x01000000).
 *
 * @par Confirmed
 * Sets flags 0x01000010. The owner resolves via func_8004D664(unk8C &
 * 0x3FFFFFFF); an owner of type 2 provides a local record (its +0x78..+0x80
 * words and +0x1A half) passed to func_8003B898(region, entity, record,
 * (flags19 >> 3) & 0x1E, 2). A non-null +0x74 triggers func_80031690.
 *
 * @par Likely role
 * The shared despawn core behind func_8002AD58 case 4 and the retire
 * handlers: the two argument bits let callers choose whether the footprint
 * and the script/sound teardown run (deferred vs immediate retirement).
 *
 * @par Unknown
 * What func_80031690 does with the hook block.
 *
 * @par Callers
 * func_8002AD58, func_8002B784, func_8002C174, func_80058D78.
 *
 * @par Environment path
 * Shared retire step of an entity: it returns 0 if flag 0x01000000 of +0x24
 * is already set, otherwise sets 0x01000010, resolves the owner from the id
 * at +0x8C with func_8004D664, and, when the entity's slot byte +0x20 is
 * non-zero and arg2 is set, calls gSpawnEntitySlotTypes (func_8003B898) in
 * mode 2 with mask (+0x19 >> 3) & 0x1E, passing a local record copied from a
 * type-2 owner (+0x78..+0x80, +0x1A). It then calls func_80031690 when +0x74
 * is non-null and func_8004D958 when arg1 is set, and returns 1.
 * gEntityBaseHandler (func_8002AD58) calls it for message 4 with (1, 1)
 * (8002AD58.c:88), and func_8002B784, func_8002C174 and func_80058D78 use it
 * from their own retire paths.
 */
s32 gRetireEntityBase(Unk8002A810 *s0, s32 arg1, s32 arg2)
{
    register Unk8002A810Sub *a0 __asm__("$4");
    Unk8002A810Local local;
    Unk8002A810Local *a2;
    s32 word7C;
    register s32 word78 __asm__("$3");
    u16 half1A;
    register u8 type __asm__("$5");

    if (s0->unk24 & 0x01000000) {
        return 0;
    }

    s0->unk24 |= 0x01000010;

    a0 = 0;
    if (s0->unk8C != 0) {
        a0 = func_8004D664(s0->unk8C & 0x3FFFFFFF);
    }

    if (s0->unk20 != 0) {
        a2 = 0;
        if (a0 != 0) {
            type = a0->unk16;
            if (type == 2) {
                local.unk49 = type;
                word7C = a0->unk7C;
                word78 = a0->unk78;
                local.unk08 = word7C;
                local.unk0C = a0->unk80;
                half1A = a0->unk1A;
                a2 = &local;
                local.unk04 = word78;
                local.unk40 = half1A;
            }
        }
        if (arg2 & 0xFF) {
            register s32 two __asm__("$2");
            two = 2;
            gSpawnEntitySlotTypes(s0->unk20, s0, a2,
                          (s0->unk19 >> 3) & 0x1E, two);
        }
    }

    if (s0->unk74 != 0) {
        func_80031690(s0);
    }

    if (arg1 & 0xFF) {
        func_8004D958(s0);
    }
    return 1;
}
