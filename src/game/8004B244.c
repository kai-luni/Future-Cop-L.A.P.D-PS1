// Categories: Physics > Motion | Gameplay > Damage
#include "game/types.h"

struct Unk4B244Source {
    u8 unk00[0x1A];
    u16 flags;
};

struct Unk4B244Force {
    s32 unk00;
    s32 x;
    s32 y;
    s32 z;
    u8 unk10[0x30];
    u16 strength;
    u8 unk42[7];
    u8 flags;
};

/**
 * @brief Computes a signed impulse vector from a force record, gated by the
 *        source's flag bits.
 *
 * @param source Object block; +0x1A the flag word.
 * @param force Force record; +4/+8/+0xC the direction triple, +0x40 the
 *        u16 strength, +0x49 the flags byte.
 * @param out Three s32 impulse components.
 * @return 1 when an impulse was written, 0 when the force is null, its flag
 *         bit 1 is clear, the source suppresses it (bit 0x200) or the
 *         strength is zero.
 *
 * @par Confirmed
 * scale = strength << 3, negated when source flag bit 0x400 is set; bit
 * 0x200 forces scale 0. Each output is (force->axis >> 4) * scale >> 8.
 *
 * @par Likely role
 * The explosion push of the debris handlers func_8003AA94/func_8004B5F8:
 * the direction comes from the blast, the sign from the object's orientation
 * flags, and the 4/8 shifts convert the force units to world velocity.
 *
 * @par Unknown
 * The unit of strength and why bit 0x200 suppresses the impulse entirely.
 *
 * @par Callers
 * func_8003AA94, func_8004B5F8.
 */
s32 func_8004B244(struct Unk4B244Source *source, struct Unk4B244Force *force, s32 *out)
{
    s16 scale;
    u16 flags;

    if (force == 0) {
        return 0;
    }
    if (!(force->flags & 2)) {
        return 0;
    }
    flags = source->flags;
    scale = 0;
    if (!(flags & 0x200)) {
        if (flags & 0x400) {
            scale = -(force->strength << 3);
        } else {
            scale = force->strength << 3;
        }
    }
    if (scale == 0) {
        return 0;
    }
    out[0] = ((force->x >> 4) * scale) >> 8;
    out[1] = ((force->y >> 4) * scale) >> 8;
    out[2] = ((force->z >> 4) * scale) >> 8;
    return 1;
}
