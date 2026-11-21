// Categories: Gameplay > Object Spawning | Resources > Spawn Tables | Graphics > Scene Graph
#include "game/types.h"

#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

struct Matrix {
    s32 m[12];
};

struct Source {
    u8 pad00[4];
    u8 kind;
    u8 pad05[0x0F];
    u8 unk14;
    u8 childType;
    u8 pad16[0x08];
    s8 unk1E;
    u8 pad1F[0x05];
    s32 unk24;
    u8 pad28[0x06];
    u16 unk2E;
    s16 unk30;
    u16 flags32;
};

struct Object {
    struct Object *next;
    u8 pad04[2];
    u8 unk06;
    u8 pad07[0x0E - 0x07];
    u8 unk0E;
    u8 flags0F;
    u8 flags10;
    u8 soundId11;
    u16 unk12;
    u8 pad14[0x1A - 0x14];
    u16 flags1A;
    void *parent1C;
    struct Source *source20;
    s32 (*func24)(s32, struct Object *, void *, s32);
    u8 pad28[0x34 - 0x28];
    struct Matrix matrix;
};

struct Parent {
    u8 pad00[0x34];
    void *holder34;
    u8 pad38;
    u8 flags39;
    u16 unk3A;
    u8 pad3C[2];
    u16 unk3E;
};

struct Data {
    u8 pad00[0x24];
    s32 x;
    s32 y;
    s32 z;
};

struct Player {
    u8 pad00[0x50];
    s32 x;
    s32 y;
    s32 z;
};

struct Target {
    u8 pad00[0x28];
    s32 x;
    s32 y;
    s32 z;
};

struct Holder;

struct DestinationTriple {
    s32 second;
    s32 zero;
    s32 first;
};

struct SpawnWork {
    struct Matrix matrix;       /* +00: retail sp+20 */
    u16 half;                   /* +30: retail sp+50 */
    s8 slot;                    /* +32: retail sp+52 */
    u8 pad33[5];
    s32 diff[4];                /* +38: retail sp+58 */
    struct DestinationTriple point; /* +48: retail sp+68 */
    s32 point_pad;
    s32 distVec[4];             /* +58: retail sp+78 */
    s32 vec1[4];                /* +68: retail sp+88 */
    s32 vec2[4];                /* +78: retail sp+98 */
};

extern void *gSpawnRecordMap[][0x100] __asm__("D_800A7040");
extern s32 gPlayerCount __asm__("D_8009FA0C");
extern struct Player *gPlayerEntities __asm__("D_8009FA58");
extern struct Player *gPlayerEntity2 __asm__("D_8009FA5C");
extern void *gSpawnMotionContext __asm__("D_8009F374");
extern struct Object *gPendingSpawnList __asm__("D_8009F314");

extern void gBuildNodePlacement(struct Source *, struct Object *, struct Parent *, void *, struct Matrix *, u16 *, s8 *) __asm__("func_8003DB70");
extern struct Data *gResolveNodeTransform(struct Holder *, u16, s8, s32, struct Parent *, struct Matrix *) __asm__("func_8003D9F8");
extern u32 *gGetSpawnTypeFlagPtr(u32, u32) __asm__("func_8003B3D8");
extern void func_8003B3F8(struct Object *, u32, s32);
extern s32 func_8003CDAC(struct Holder *, s8);
extern void gGetViewGroundAnchor(u32, struct DestinationTriple *) __asm__("func_80069580");
extern s32 gVectorLength3D(s32 *) __asm__("func_8001A100");
extern s32 gApproxVectorLength(s32 *) __asm__("func_80019F54");
extern void func_80057358(void *, s32);
extern void func_80025D6C(s32 *);
extern void gApplyRadialContactDamage(s32 *, u16, s16, void *) __asm__("func_800410BC");
extern struct Object *gCreateLightObject(struct Source *) __asm__("func_80040E60");
extern struct Object *gCreateKind59Actor(struct Source *) __asm__("func_8007043C");
extern struct Object *func_8006EB80(struct Source *);
extern struct Object *func_800447F0(struct Source *);
extern struct Object *func_8004C920(struct Source *);
extern struct Object *gCreateEmitterObject(struct Source *) __asm__("func_800762B0");
extern struct Object *func_80074024(struct Source *);
extern s32 gPlayNonpositionalSoundEffect(s32, s32) __asm__("func_80046C84");
extern void gPlayPositionalSoundEffect(s32, s32, s32, s32) __asm__("func_80047284");
s32 gSpawnObjectFromTable(struct Parent *parent, struct Object *link, u8 type, void *arg3, s32 arg4) __asm__("func_8003E984");


/**
 * @brief Spawns one game object from the static spawn tables: resolves the
 *        source record, tests whether a player is near, builds the placement
 *        matrix, runs the type-specific constructor and the object's own init
 *        callback, then optionally spawns the source's child type recursively.
 *
 * @param parent Parent object, or null. When set, its +0x34 is used as the
 *               holder for the transform, its +0x3A child counter and its
 *               +0x3E "visible child" counter are incremented, and bit 3 of its
 *               +0x39 flags is set for children that carry flag 0x40.
 * @param link   The object that requested the spawn. Only read for the
 *               duplicate test: when link->+0x20 -> +0x14 equals @p type and
 *               link->+0x06 is zero, the child chain is skipped.
 * @param type   Column index into the spawn table (0..0xFF). 0x5D, 0x59, 0x5C,
 *               0x5E, 0x5A, 0x57 and 0x58 select a constructor; 0x5B and any
 *               other value fall through to "no object".
 * @param arg3   Opaque argument forwarded unchanged to the object's init
 *               callback func24(0, object, arg3, 0).
 * @param arg4   Row index 0 or 1 into the spawn table. Row 1 marks the new
 *               object with flags10 |= 2.
 *
 * @return The new object pointer on success, 0 when the source is missing, no
 *         player is within range, the constructor returned null or the init
 *         callback failed. Note the function's own declaration is s32; the
 *         callers use the result as a pointer.
 *
 * @par Globals (confirmed layout)
 * Source: kind +0x04, childType +0x15, unk1E +0x1E (s8), unk24 +0x24,
 * unk2E (u16) +0x2E, unk30 (s16) +0x30, flags32 (u16) +0x32, and unk14 (u8) -
 * the field the duplicate test compares against @p type.
 * Object: next +0x00, unk06 +0x06, unk0E +0x0E, flags0F +0x0F,
 * flags10 +0x10, soundId11 +0x11, unk12 (u16) +0x12, flags1A (u16) +0x1A,
 * parent1C +0x1C, source20 +0x20, func24 callback +0x24, matrix (12 words)
 * +0x34.
 * Parent: holder34 +0x34, flags39 +0x39, unk3A (u16) +0x3A, unk3E (u16) +0x3E.
 * Data: x/y/z at +0x24/+0x28/+0x2C (the same +0x24 the matrix translation
 * occupies, which is why the motion call below receives &data->x).
 *
 * @par Control flow (confirmed)
 * 1. source = D_800A7040[arg4][type] - a 2-D table of 0x100 pointers per row,
 *    so the row stride is 0x400 bytes. A null entry returns 0 immediately.
 * 2. holder = parent ? parent->holder34 : 0.
 * 3. func_8003DB70(source, link, parent, arg3, &matrix, &half, &slot) fills the
 *    placement matrix, a half-word and a slot byte; func_8003D9F8(holder,
 *    flags32, unk1E, 0, parent, &matrix) resolves the world position.
 * 4. Distance gate: unless (byte at func_8003B3D8(type, arg4)) & 5 is non-zero,
 *    the loop over i < D_8009FA0C calls func_80069580(i, &point) to fetch player
 *    i's position and accepts the spawn when func_8001A100(delta) <= 0x1FFFF
 *    for at least one player. No player in range means no object.
 * 5. switch (source->kind) dispatches to the per-kind constructor:
 *    0x5D -> func_80040E60, 0x59 -> func_8007043C, 0x5C -> func_8006EB80,
 *    0x5E -> func_800447F0, 0x5A -> func_8004C920, 0x57 -> func_800762B0,
 *    0x58 -> func_80074024, 0x5B and default -> null.
 * 6. On success: source20 = source, func_8003B3F8(object, type, arg4) registers
 *    the object in the same table, arg4 == 1 sets flags10 bit 1, parent1C =
 *    parent and the parent counters are bumped.
 * 7. The slot is validated with func_8003CDAC(holder, slot); failure forces
 *    0xFF. It is then stored in object +0x0E.
 * 8. The 12-word matrix is copied word by word (+0x34..+0x60) and object +0x12
 *    receives the half-word.
 * 9. object->func24(0, object, arg3, 0) is the object's own init callback; a
 *    zero return runs func24(10, object, 0, 0) as a teardown and fails the
 *    spawn.
 * 10. flags1A & 0x2010 triggers an alert payment: 0x3E8 when bit 0x2000 is
 *     set, otherwise 0x12C, paid with func_80057358 to each player within
 *     0x1FFFF after an ABS of func_80019F54.
 * 11. flags1A & 0x20 calls func_80025D6C(&data->x).
 * 12. source->unk2E != 0: with unk30 > 0 func_800410BC(&data->x, unk2E, unk30,
 *     D_8009F374) starts a motion, otherwise the object receives callback
 *     command 9 (abort).
 * 13. The object is pushed onto the global list head D_8009F314.
 * 14. soundId11 != 0 emits the creation sound: for types 0x3D..0x44 and
 *     0x64..0x76 the nearest of the (one or two) players within 0x1FFFF gets
 *     func_80046C84(id, player), all other types get
 *     func_80047284(id, x, z, -1) with the object position (from the holder
 *     when parent is set, otherwise from matrix words 9/11).
 * 15. Child chain: skip when the duplicate test hits, or source->unk24 != 0, or
 *     childType == 0, or flags32 & 0x800. Otherwise this function recurses with
 *     (parent, object, childType, 0, arg4), then clears object->unk06 and sets
 *     flags0F bit 1.
 *
 * @par Globals read/written
 * D_800A7040: spawn table (read). D_8009FA0C: player count (read).
 * D_8009FA58 / D_8009FA5C: the two player records (read).
 * D_8009F374: motion argument (read). D_8009F314: global object list head
 * (written - the new object becomes the head).
 *
 * @par Calls
 * func_8003B3D8 / func_8003B3F8 (table slot lookup / registration),
 * func_8003DB70 (placement matrix), func_8003D9F8 (world position),
 * func_8003CDAC (slot validity), func_80069580 (player position),
 * func_8001A100 and func_80019F54 (distance), func_80057358 (alert payment),
 * func_80025D6C, func_800410BC (start motion), func_80046C84 / func_80047284
 * (sound by id), the seven constructors, and itself.
 *
 * @par Side effects
 * Creates the object, links it into D_8009F314, registers it in the spawn
 * table, increments the parent's child counters, may spawn a whole child chain
 * recursively and may trigger sounds and alert payments.
 *
 * @par Callers
 * Direct, from func_8003E984 itself (recursive child spawn), func_8003F2AC,
 * func_8003FE0C, func_800402DC, func_8004ABE4, func_8004B180 and func_8004B5F8.
 * The latter five all pass the source type as a byte and the row as
 * (flags >> 1) & 1, which is how the two spawn-table rows are chosen.
 *
 * @par Likely
 * This is the generic "instantiate a level object" entry point: the table is
 * indexed by object kind, the distance gate avoids creating objects in parts of
 * the level the players are not near, and the child chain builds the pieces of a
 * composite object (e.g. a vehicle with its turret).
 *
 * @par Unknown
 * - The concrete meaning of the seven source kinds 0x57..0x5E; they are only
 *   distinguished by which constructor they call.
 * - What bit 5 of the byte at func_8003B3D8(type, arg4) means (it disables the
 *   player distance gate) and what bit 0 means.
 * - The meaning of source->unk24, which suppresses the child chain.
 * - Whether D_8009F314 is a plain list head or a per-frame render list; only
 *   head insertion is visible here.
 *
 * @par Environment path
 * Instantiates one object from definition gSpawnRecordMap[bank][type]
 * (D_800A7040, line 263): it builds the placement matrix with
 * gBuildNodePlacement (func_8003DB70) and resolves the world position with
 * gResolveNodeTransform (func_8003D9F8); unless the type's flag byte from
 * func_8003B3D8 has bit 0 or 2, the spawn is rejected when no
 * gGetViewGroundAnchor point of the gPlayerCount views lies within 0x1FFFF
 * (lines 276-290). It then calls the constructor for source->kind (0x57
 * func_800762B0, 0x58 func_80074024, 0x59 func_8007043C, 0x5A func_8004C920,
 * 0x5C func_8006EB80, 0x5D func_80040E60, 0x5E func_800447F0), registers the
 * object with func_8003B3F8, bumps the parent anchor's counters +0x3A/+0x3E,
 * copies the matrix, runs the init callback func24(0, ...), calls
 * func_80057358 with 0x3E8 or 0x12C for each player within range when flags1A
 * & 0x2010, may start a motion with func_800410BC and D_8009F374, pushes the
 * object onto gPendingSpawnList (D_8009F314), plays its creation sound and
 * recursively spawns the definition's child type. Callers are
 * gSpawnTypeAtOwner (func_800402DC), gLaunchDebrisObject (func_8004ABE4),
 * func_8003F2AC, func_8003FE0C, func_8004B180, func_8004B5F8 and itself;
 * gRunEntityWorldTick (func_8003F570) drains the pending list at
 * 8003F570.c:109-112.
 */
s32 gSpawnObjectFromTable(struct Parent *parent, struct Object *link, u8 type, void *arg3, s32 arg4)
{
    struct SpawnWork work;
    register struct Source *source __asm__("$21");
    register struct Data *data __asm__("$20");
    register struct Object *object __asm__("$18");
    struct Object *prev;
    struct Object *tmp;
    register struct Holder *holder __asm__("$19");
    struct Target *target;
    u8 found;
    register s32 i __asm__("$16");
    register s32 amount __asm__("$22");
    s32 dist;
    s32 dist2;
    register s32 last_matrix_word __asm__("$8");

    source = (struct Source *)gSpawnRecordMap[arg4][type];
    if (source == 0) {
        goto failed;
    }

    holder = 0;
    if (parent != 0) {
        holder = (struct Holder *)parent->holder34;
    }

    gBuildNodePlacement(source, link, parent, arg3, &work.matrix, &work.half, &work.slot);
    data = gResolveNodeTransform(holder, source->flags32, source->unk1E, 0, parent, &work.matrix);

    if ((*(u8 *)gGetSpawnTypeFlagPtr(type, arg4) & 5) == 0) {
        found = 0;
        for (i = 0; i < gPlayerCount; i++) {
            gGetViewGroundAnchor(i, &work.point);
            work.diff[0] = data->x - work.point.second;
            work.diff[1] = data->y - work.point.zero;
            work.diff[2] = data->z - work.point.first;
            if (gVectorLength3D(work.diff) <= 0x1FFFF) {
                found = 1;
            }
        }
        if (!found) {
            goto failed;
        }
    }

    switch (source->kind) {
    case 0x5D:
        tmp = gCreateLightObject(source);
        break;
    case 0x59:
        tmp = gCreateKind59Actor(source);
        break;
    case 0x5C:
        tmp = func_8006EB80(source);
        break;
    case 0x5E:
        tmp = func_800447F0(source);
        break;
    case 0x5A:
        tmp = func_8004C920(source);
        break;
    case 0x57:
        tmp = gCreateEmitterObject(source);
        break;
    case 0x58:
        tmp = func_80074024(source);
        break;
    case 0x5B:
    default:
        tmp = 0;
        break;
    }
    object = tmp;
    if (object == 0) {
        goto failed;
    }

    object->source20 = source;
    func_8003B3F8(object, type, arg4);
    if (arg4 == 1) {
        object->flags10 |= 2;
    }

    object->parent1C = parent;
    if (parent != 0) {
        parent->unk3A++;
        if (!(object->flags10 & 4)) {
            parent->unk3E++;
        }
    }

    if (holder != 0) {
        if ((u8)func_8003CDAC(holder, work.slot) == 0) {
            work.slot = 0xFF;
        }
    }
    object->unk0E = work.slot;

    object->matrix.m[0] = work.matrix.m[0];
    object->matrix.m[1] = work.matrix.m[1];
    object->matrix.m[2] = work.matrix.m[2];
    object->matrix.m[3] = work.matrix.m[3];
    object->matrix.m[4] = work.matrix.m[4];
    object->matrix.m[5] = work.matrix.m[5];
    object->matrix.m[6] = work.matrix.m[6];
    object->matrix.m[7] = work.matrix.m[7];
    object->matrix.m[8] = work.matrix.m[8];
    object->matrix.m[9] = work.matrix.m[9];
    object->matrix.m[10] = work.matrix.m[10];
    last_matrix_word = work.matrix.m[11];
    object->unk12 = work.half;
    object->matrix.m[11] = last_matrix_word;

    if (object->func24(0, object, arg3, 0) == 0) {
        object->func24(10, object, 0, 0);
        goto failed;
    }

    if (object->flags1A & 0x2010) {
        amount = (object->flags1A & 0x2000) ? 0x3E8 : 0x12C;

        work.distVec[0] = gPlayerEntities->x - data->x;
        work.distVec[1] = gPlayerEntities->y - data->y;
        work.distVec[2] = gPlayerEntities->z - data->z;
        if (ABS(gApproxVectorLength(work.distVec)) <= 0x1FFFF) {
            func_80057358(gPlayerEntities, amount);
        }

        if (gPlayerCount >= 2) {
            work.distVec[0] = gPlayerEntity2->x - data->x;
            work.distVec[1] = gPlayerEntity2->y - data->y;
            work.distVec[2] = gPlayerEntity2->z - data->z;
            if (ABS(gApproxVectorLength(work.distVec)) <= 0x1FFFF) {
                func_80057358(gPlayerEntity2, amount);
            }
        }
    }

    if (object->flags1A & 0x20) {
        func_80025D6C(&data->x);
    }

    if (object->source20->unk2E != 0) {
        if (object->source20->unk30 > 0) {
            gApplyRadialContactDamage(&data->x, object->source20->unk2E, object->source20->unk30, gSpawnMotionContext);
        } else {
            object->func24(9, object, 0, 0);
        }
    }

    prev = gPendingSpawnList;
    gPendingSpawnList = object;
    object->next = prev;

    if (object->soundId11 != 0) {
        target = 0;
        if (object->parent1C != 0) {
            target = (struct Target *)((struct Parent *)object->parent1C)->holder34;
        }

        if ((type >= 0x3D && type < 0x45) || (type >= 0x64 && type < 0x77)) {
            if (gPlayerCount >= 2) {
                if (target != 0) {
                    work.vec1[0] = target->x - gPlayerEntities->x;
                    work.vec1[1] = target->y - gPlayerEntities->y;
                    work.vec1[2] = target->z - gPlayerEntities->z;
                    work.vec2[0] = target->x - gPlayerEntity2->x;
                    work.vec2[1] = target->y - gPlayerEntity2->y;
                    work.vec2[2] = target->z - gPlayerEntity2->z;
                } else {
                    work.vec1[0] = object->matrix.m[9] - gPlayerEntities->x;
                    work.vec1[1] = object->matrix.m[10] - gPlayerEntities->y;
                    work.vec1[2] = object->matrix.m[11] - gPlayerEntities->z;
                    work.vec2[0] = object->matrix.m[9] - gPlayerEntity2->x;
                    work.vec2[1] = object->matrix.m[10] - gPlayerEntity2->y;
                    work.vec2[2] = object->matrix.m[11] - gPlayerEntity2->z;
                }
                dist = gApproxVectorLength(work.vec1);
                dist2 = gApproxVectorLength(work.vec2);
                if (dist < dist2) {
                    if (dist <= 0x1FFFF) {
                        gPlayNonpositionalSoundEffect(object->soundId11, (s32)gPlayerEntities);
                    } else if (dist2 <= 0x1FFFF) {
                        gPlayNonpositionalSoundEffect(object->soundId11, (s32)gPlayerEntity2);
                    }
                }
            } else {
                if (target != 0) {
                    work.vec1[0] = target->x - gPlayerEntities->x;
                    work.vec1[1] = target->y - gPlayerEntities->y;
                    work.vec1[2] = target->z - gPlayerEntities->z;
                } else {
                    work.vec1[0] = object->matrix.m[9] - gPlayerEntities->x;
                    work.vec1[1] = object->matrix.m[10] - gPlayerEntities->y;
                    work.vec1[2] = object->matrix.m[11] - gPlayerEntities->z;
                }
                dist = gApproxVectorLength(work.vec1);
                if (dist <= 0x1FFFF) {
                    gPlayNonpositionalSoundEffect(object->soundId11, (s32)gPlayerEntities);
                }
            }
        } else {
            if (target != 0) {
                gPlayPositionalSoundEffect(object->soundId11, target->x, target->z, -1);
            } else {
                gPlayPositionalSoundEffect(object->soundId11, object->matrix.m[9], object->matrix.m[11], -1);
            }
        }
    }

    if (parent != 0) {
        if (object->flags1A & 0x40) {
            parent->flags39 |= 8;
        }
    }

    if (link != 0 && link->source20->unk14 == type && link->unk06 == 0) {
        goto success;
    }

    if (source->unk24 != 0) {
        goto success;
    }
    if (source->childType == 0) {
        goto success;
    }
    if (source->flags32 & 0x800) {
        goto success;
    }

    gSpawnObjectFromTable(parent, object, source->childType, 0, arg4);
    object->unk06 = 0;
    object->flags0F |= 2;
success:
    return (s32)object;
failed:
    return 0;
}
