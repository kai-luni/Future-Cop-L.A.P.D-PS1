// Categories: Gameplay > Weapons > Spawning | Gameplay > Combat
#include "game/types.h"

extern int gWeaponFireDispatch();

struct Unk5EE04Vec {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk5EE04Ent {
    u8 unk00[0x14];
    u16 unk14;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19[0x0B];
    u32 unk24;
    u8 unk28[0x50];
    struct Unk5EE04Vec unk78;
    u32 unk84;
};

struct Unk5EE04Obj {
    u16 unk00;
    s16 unk02;
    u8 unk04;
    u8 unk05;
    s16 unk06;
    u8 unk08;
    u8 unk09;
    s16 unk0A;
    s16 unk0C;
    u8 unk0E;
    u8 unk0F[3];
    u8 unk12;
};

struct Unk5EE04Args {
    u32 flags;
    u8 kind;
    u8 unk05[3];
    struct Unk5EE04Vec position;
    u32 unk14;
    u16 unk18;
    u16 unk1A;
    u8 unk1C;
    u8 unk1D;
    u8 unk1E;
    u8 unk1F;
    u8 unk20;
    u8 unk21;
    u8 unk22;
    u8 unk23;
    u32 unk24;
    struct Unk5EE04Vec unk28;
    u32 unk34;
    u32 unk38;
    u32 unk3C;
    u32 unk40;
    u32 unk44;
    u8 unk48;
    u8 unk49;
    u16 unk4A;
};

struct Unk5EE04Request {
    void (*handler)();
    u32 unk04;
    struct Unk5EE04Args *args;
    u32 unk0C;
};

extern void gKind15ProjectileHandler() __asm__("D_80037708");
extern s32 func_80019668(void);
extern void gVectorScaleInPlace(s32 value, void *data) __asm__("func_8001AB78");
extern void gSpawnQueuedEntity(struct Unk5EE04Request *request) __asm__("func_8004D444");
void gSpawnJitterProjectile(struct Unk5EE04Obj *obj, s32 **ptr, struct Unk5EE04Vec *pos,
                   struct Unk5EE04Ent *ent, struct Unk5EE04Vec *arg4, u8 flag, u32 flags) __asm__("func_8005EE04");


/**
 * @brief Spawns a kind-0xF projectile with jittered speed and inherited or
 *        solved velocity (handler func_80037708).
 *
 * @param obj Weapon descriptor (speed words +2/+0xA, jitter base +0xC).
 * @param ptr Pointer to the owner pointer.
 * @param pos Launch position.
 * @param ent Firing entity; +0x78 the velocity, +0x84 the inherit scale.
 * @param arg4 Fallback direction vector.
 * @param flag Non-zero skips the velocity scaling.
 * @param flags Base flag word.
 * @return Nothing (the spawn is queued via gSpawnQueuedEntity).
 *
 * @par Confirmed
 * Standard flag composition (|0x8000, +0x4000 for kind 0x12, unk14 = flags
 * | 8 with the 0x100 promotion). kind = 0xF, lifetime unk18 = 0x4E20.
 * Speed unk40 = v + v * (rand & 0x1F) / 64 (up to +48% jitter). Velocity:
 * without flag and with ent->unk84 set, the entity velocity scaled by
 * unk84 << 8 (func_8001AB78); else arg4, scaled by obj->unk06 when the flag
 * is clear.
 *
 * @par Likely role
 * The rapid-fire/spread projectile path of the fire dispatchers: the
 * lifetime 0x4E20 (20000) is a long-lived round, the speed jitter spreads
 * bursts, inheriting the platform velocity keeps shots consistent while
 * moving.
 *
 * @par Callers
 * func_8005F6E8, gWeaponFireDispatch.
 */
void gSpawnJitterProjectile(struct Unk5EE04Obj *obj, s32 **ptr, struct Unk5EE04Vec *pos,
                   struct Unk5EE04Ent *ent, struct Unk5EE04Vec *arg4, u8 flag, u32 flags)
{
    register struct Unk5EE04Vec *po __asm__("$20");
    register struct Unk5EE04Ent *e __asm__("$17");
    register u8 fl __asm__("$19");
    register struct Unk5EE04Obj *o __asm__("$18");
    s32 v;
    register u32 fs __asm__("$6");
    register u32 base __asm__("$4");
    register struct Unk5EE04Obj *input __asm__("$4");
    struct Unk5EE04Request request;
    struct Unk5EE04Args args;

    input = obj;
    po = pos;
    e = ent;
    __asm__("" : "+r"(po), "+r"(e));
    fs = flags;
    fl = flag;

    request.handler = gKind15ProjectileHandler;
    request.unk04 = 0;
    request.args = &args;
    request.unk0C = 0;

    {
        register u32 tag __asm__("$2") = e->unk14;
        __asm__("addu\t%0,%1,$zero" : "=r"(o) : "r"(input), "r"(tag));
        base = tag | 0x8000;
        __asm__("" : "+r"(base));
        args.flags = base;
    }
    if (e->unk16 == 0x12) {
        args.flags = base | 0x4000;
    }
    __asm__("" : : "r"(base));
    args.unk1D = e->unk17;
    args.unk1C = e->unk18;
    args.unk14 = fs | 0x8;
    if (e->unk24 & 0x100) {
        args.unk14 = fs | 0x108;
    } else {
        args.unk14 &= ~0x100;
    }
    __asm__("" : : "r"(fs));
    args.kind = 0xF;
    args.unk18 = 0x4E20;
    args.unk1A = o->unk00;
    args.unk38 = o->unk02 << 4;
    args.unk34 = o->unk0A << 4;
    args.unk1E = 0;
    args.unk1F = 0;
    args.unk22 = 0;
    args.unk23 = 0;
    args.unk24 = **ptr;
    args.unk3C = o->unk12;
    args.unk48 = o->unk0E;
    args.unk20 = 0;
    args.unk21 = o->unk12;
    args.unk4A = 0;
    v = o->unk0C;
    args.unk40 = v + (v * (func_80019668() & 0x1F)) / 64;
    args.unk44 = (u32)e;
    args.position = *po;

    if (!fl && e->unk84 != 0) {
        args.unk28 = e->unk78;
        gVectorScaleInPlace(e->unk84 << 8, &args.unk28);
    } else {
        args.unk28 = *arg4;
        if (!fl) {
            gVectorScaleInPlace(o->unk06, &args.unk28);
        }
    }

    gSpawnQueuedEntity(&request);
}
