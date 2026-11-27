// Categories: Gameplay > Collision | Physics > Proximity Test
#include "game/types.h"

#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

struct Unk8002FF6C {
    u8 unk00[0x16];
    u8 unk16;
    u8 unk17[0x50 - 0x17];
    s32 x;
    s32 y;
    s32 z;
    u8 unk5C[0xB4 - 0x5C];
    s32 unkB4;
    s32 unkB8;
    s32 unkBC;
    u8 unkC0[0xC8 - 0xC0];
    s32 unkC8;
    u8 unkCC[4];
    u8 unkD0[4];
};

extern void gApplyPlayerResupply(struct Unk8002FF6C *other, s32 arg1, s32 arg2,
                          u8 *arg3, u8 *arg4) __asm__("func_8002FC90");
extern void gRetireEntity(struct Unk8002FF6C *self) __asm__("func_8004D958");
s32 gApplyRotatingPropProximityEffect(struct Unk8002FF6C *self, struct Unk8002FF6C *other) __asm__("func_8002FF6C");


/**
 * @brief Axis-aligned proximity test that applies an effect on contact.
 *
 * A target of type 0x12 is a proxy and is dereferenced through its 0xB4
 * pointer first; anything that is not type 1 is ignored. The three axis
 * deltas between self and target are compared against the self box half
 * extents at 0xB4/0xB8/0xBC. When all three fit, func_8002FC90 is called with
 * the self payload word 0xC8, selector -1 and the two 0xCC/0xD0 values, and
 * the caller itself is retired when its flag 0x02000000 is set.
 *
 * @param self Entity with the box half extents and the payload words.
 * @param other Target entity, or a proxy that points at the real target.
 * @return Always 0.
 *
 * @par Globals
 * None; the box and payload live in the entity block.
 *
 * @par Control flow
 * Nested if chain, one branch per axis, so the effect only fires on a full hit.
 *
 * @par Calls
 * func_8002FC90 - applies the payload to the target. func_8004D958 - retires self.
 *
 * @par Side effects
 * May modify the target through the payload call and retire the caller.
 *
 * @par Callers
 * Confirmed: func_8003015C.
 */
s32 gApplyRotatingPropProximityEffect(struct Unk8002FF6C *self, struct Unk8002FF6C *other)
{
    s32 dx;
    s32 dy;
    s32 dz;

    if (other->unk16 == 0x12) {
        other = (struct Unk8002FF6C *)other->unkB4;
    }
    if (other->unk16 != 1) {
        return 0;
    }
    dx = self->x - other->x;
    dy = self->y - other->y;
    dz = self->z - other->z;
    if (ABS(dx) < self->unkB4) {
        if (ABS(dy) < self->unkB8) {
            if (ABS(dz) < self->unkBC) {
                gApplyPlayerResupply(other, self->unkC8, -1, self->unkD0, self->unkCC);
                if (self->unkC8 & 0x2000000) {
                    gRetireEntity(self);
                }
            }
        }
    }
    return 0;
}
