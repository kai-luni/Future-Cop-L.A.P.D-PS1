// Categories: Gameplay > Entity Update | Physics > Motion | Gameplay > Object Spawning
#include "game/types.h"

struct Vec3s32 { s32 x; s32 y; s32 z; };
struct EntityLink { u8 pad[8]; s32 unk08; };
struct Entity28 { u8 pad[0x34]; u8 *unk34; };

struct Entity {
    s32 unk00;
    s32 unk04;
    struct Entity *unk08;
    u16 unk0C;
    u8 unk0E[6];
    u16 unk14;
    u8 unk16[2];
    u8 unk18;
    u8 unk19[3];
    s16 unk1C;
    u8 unk1E[4];
    s8 unk22;
    u8 unk23[5];
    struct Entity28 *unk28;
    u8 unk2C[0x24];
    struct Vec3s32 unk50;
    u8 unk5C[0x14];
    s16 unk70;
    u8 unk72;
    u8 unk73[5];
    struct Vec3s32 unk78;
    s32 unk84;
    u8 unk88[0x18];
    struct Vec3s32 unkA0;
    s32 unkAC;
    u8 unkB0[4];
    u8 unkB4;
    u8 unkB5;
    u8 unkB6[0xA];
    struct EntityLink *unkC0;
    s32 unkC4;
    s32 unkC8;
    u8 unkCC[4];
    s32 unkD0;
};

extern void gVectorAddScaled(void *a, void *b, s32 c, s32 *d) __asm__("func_8001ACA4");
extern s32 func_800684AC(void *a, s32 *b, s32 c, s32 d, s32 e);
extern s32 func_8001A100(s32 *vector);
extern void func_8001A180(struct Vec3s32 *vector);
extern struct Entity *func_8004D3FC(s32 size);
extern s32 func_80019668(void);
extern s32 func_800402DC(void *owner, u8 column, s32 arg2, s32 row, s32 arg4);

extern void func_80036940(struct Entity *self, s32 arg2, s32 arg3);
extern void func_8001937C(struct Vec3s32 *axis, void *basis);
extern void func_800193F8(struct Vec3s32 *axis, void *basis);
extern s32 func_8001A750(struct Vec3s32 *a, void *b);
extern void func_8004D958(struct Entity *entity);
extern void func_80010750(void *p);
extern void func_80010234(void *a, void *b, s32 c);
extern void func_80036AB4(struct Entity *self, s32 arg3);
extern s32 func_8002AD58(s32 message, struct Entity *self, s32 arg2, s32 arg3);
extern void *jtbl_80011380[];

/**
 * @brief Per-frame motion of a projectile-like entity: advances it along its
 *        motion vector with a downward bias, cancels the step when it would pass
 *        through geometry, refreshes the heading and speed, and spawns four
 *        children when the vertical component crosses zero.
 *
 * @param self Entity record: motion vector at +0xA0, descent rate at +0xAC,
 *        position at +0x50, heading at +0x78, speed at +0x84, movement flag at
 *        +0x1C, class byte at +0xB4, spawn column at +0xB5, cached link at
 *        +0xC0 with its id at +0xC4, hit flag at +0x72.
 * @param arg1 Frame delta - the second message argument, which the dispatcher
 *        forwards unchanged. Every rate in the body is divided by 300.
 * @param arg2 Unused by the body.
 * @return Nothing.
 *
 * @par Confirmed
 * A cached link is dropped when self->+0xC0 does not exist or its +0x08 no
 * longer matches self->+0xC4.
 * Gravity: old = +0xA0.y, new = old - (+0xAC * arg1) / 300, stored back; the
 * crossing flag is set only when old was non-negative and new is negative, i.e.
 * on the frame the vertical component turns downward.
 * Swept move: gVectorAddScaled(&self->+0x50, &self->+0xA0, arg1 * 218, tmp) gives
 * position + motion * (arg1 * 218) in 16.16. When func_800684AC(&self->+0x50,
 * tmp, 0, 0, 1) reports a hit the step is refused: +0x1C = -1 and bit 3 of +0x72
 * is set; otherwise +0x50 takes the three words from tmp.
 * Heading: +0x78 = +0xA0, +0x84 = func_8001A100(&+0x78) / 300 and func_8001A180
 * normalises +0x78 in place, so +0x78/+0x84 end up as unit heading and speed in
 * the units the rest of the entity code expects.
 * Burst: when +0xB4 == 20 and the crossing flag is set, four children are taken
 * from func_8004D3FC(0xD4) - the pool that stamps a serial id - and each one
 * copies the whole 0xD4-byte record first, saving and restoring the pool's two
 * header words at +0x08 and +0x00, then sets +0xB4 = 4 and +0x22 = -1 and
 * jitters the motion vector: x and z get +-0x2000 alternating by bit 0 and bit 1
 * of the loop index plus a random 14-bit centred term, y loses a random 13-bit
 * term plus (index << 13). When +0xB5 is non-zero each child is registered with
 * func_800402DC(child, +0xB5, 0, 1, 0) - the map-cell spawn gate.
 *
 * @par Likely role
 * The message-3 (tick) body of the jump-table entity class whose message 2
 * loads +0xA0 from spawn data +0x28 and +0xAC from +0x34 (func_80036940), so
 * +0xA0 is the motion vector and +0xAC its downward acceleration. Class 20 then
 * behaves like a cluster munition or a bursting shell: at the moment its vertical
 * motion turns downward it releases four class-4 children into the map cell
 * recorded at +0xB5. The collision path merely stops the motion (+0x1C = -1,
 * bit 3 of +0x72 set); nothing here destroys the entity.
 *
 * @par Unknown
 * Whether +0xA0 is a velocity or a look-ahead point (only its use here suggests a
 * velocity), why the step is scaled by 218 while everything else uses 300, and
 * what class ids 20 and 4 mean.
 *
 * @par Callers
 * func_80036E6C (message 3 of the class message table); func_80036E6C itself has
 * no direct callers because it is reached through a handler table.
 */
void func_80036BD4(struct Entity *self, s32 arg1, s32 arg2)
{
    s32 tmp[3];
    struct Entity *new;
    s32 i;
    s32 oldA4;
    s32 valA4;
    struct Entity *saved08;
    s32 saved00;

    if (self->unkC0 != 0 && self->unkC0->unk08 != self->unkC4) {
        self->unkC0 = 0;
    }

    i = 0;
    oldA4 = self->unkA0.y;
    valA4 = oldA4 - (self->unkAC * arg1) / 300;
    self->unkA0.y = valA4;
    if (oldA4 >= 0) {
        i = (u32)valA4 >> 31;
    }

    gVectorAddScaled(&self->unk50, &self->unkA0, arg1 * 218, tmp);
    if (func_800684AC(&self->unk50, tmp, 0, 0, 1) != 0) {
        self->unk1C = -1;
        self->unk72 |= 8;
    } else {
        self->unk50 = *(struct Vec3s32 *)tmp;
    }

    self->unk78 = self->unkA0;
    self->unk84 = func_8001A100((s32 *)&self->unk78) / 300;
    func_8001A180(&self->unk78);

    if (self->unkB4 == 20 && (i & 0xFF) != 0) {
        for (i = 0; i < 4; i++) {
            new = func_8004D3FC(0xD4);
            if (new != 0) {
                saved08 = new->unk08;
                saved00 = new->unk00;
                *new = *self;
                new->unkB4 = 4;
                new->unk08 = saved08;
                new->unk00 = saved00;
                new->unk22 = -1;

                new->unkA0.x += ((i & 1) ? 0x2000 : -0x2000) +
                                ((func_80019668() & 0x3FFF) - 0x2000);
                new->unkA0.y -= (func_80019668() & 0x1FFF) + (i << 13);
                new->unkA0.z += ((i & 2) ? 0x2000 : -0x2000) +
                                ((func_80019668() & 0x3FFF) - 0x2000);

                if (self->unkB5 != 0) {
                    func_800402DC(new, self->unkB5, 0, 1, 0);
                }
            }
        }
    }
}
