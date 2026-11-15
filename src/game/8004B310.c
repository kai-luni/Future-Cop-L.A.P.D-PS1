// Categories: Graphics > Effects > Debris Spawner | Physics > Motion | Utilities > Random Numbers
#include "game/types.h"

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec4B310;

typedef struct {
    u8 unk00[0x66];
    s16 unk66;
    s16 unk68;
    s16 unk6A;
    s16 unk6C;
    s16 unk6E;
    s16 unk70;
    s16 unk72;
    s16 unk74;
    s16 unk76;
    s16 unk78;
    u8 unk7A;
    u8 unk7B;
    s8 unk7C;
} Obj4B310;

extern s32 func_80019668(void);
extern void func_8001A180(Vec4B310 *arg);

/**
 * @brief Samples the launch parameters of one debris particle: two
 *        random angles, a negative acceleration term and a velocity
 *        built from a scaled direction plus a fixed base offset.
 *
 * @param arg0 Configuration. Value ranges at +0x66/+0x68 (velocity
 *        scale), +0x6A/+0x6C (offset scale) and +0x76/+0x78 (angles),
 *        base direction at +0x70/+0x72/+0x74, percentage +0x7C, flags
 *        +0x7B, byte +0x7A.
 * @param arg1 Direction vector; normalised in place by func_8001A180
 *        before it is scaled.
 * @param arg2 Extra offset triple, added only when the low byte of arg3
 *        is non-zero.
 * @param arg3 Flags; only the low byte is tested.
 * @param arg4 First angle output (s16).
 * @param arg5 Second angle output (s16), independently sampled.
 * @param arg6 Acceleration output: ((random & 0x1F) + 0xA0) << 3,
 *        re-scaled by the percentage +0x7C and by 16, always negative.
 * @param arg7 Velocity output (x, y, z).
 * @return Nothing.
 *
 * @par Confirmed
 * Angles: s0 = +0x78 - +0x76. When positive, each output receives
 * +0x76 + (s0 * (random & 0xFFF)) / 4096 from its own random sample;
 * otherwise both receive +0x78.
 * Acceleration: *arg6 = ((random & 0x1F) + 0xA0) << 3 and is then
 * recomputed as -((value * +0x7C) / 100) * 16, so it is strictly
 * negative and proportional to the +0x7C percentage.
 * Velocity: func_8001A180(arg1) normalises the direction, s0 = +0x68 -
 * +0x66 randomised over the 12-bit window when positive and +0x68
 * otherwise, then arg7->x/y/z = (arg1->x/y/z >> 4) * s0 >> 8, after
 * which arg7->y is forced non-negative.
 * Offset: s0 = +0x6C - +0x6A may be 0; when it is not, (+0x70, +0x72,
 * +0x74) * s0 >> 8 is added componentwise.
 * Finally the low byte of arg3 adds arg2 componentwise, and +0x7B mask
 * 0x04 forces arg7->y = 0.
 *
 * @par Likely role
 * The per-part initialiser of the debris spawners: func_8004B5F8 calls
 * it for every render node it creates, feeding the node's angle words
 * +0x30/+0x32, the scalar +0x40 and the velocity triple +0x34, and
 * func_8003AA94 uses it for the model parts of a particle emitter. The
 * ranges are per-class debris parameters, the negative scalar is a
 * downward acceleration or spin rate, and the +0x7B mask 0x04 option
 * flattens the vertical component for debris that has to slide.
 *
 * @par Unknown
 * The units of the two angles and of the velocity (a 16.16 value halved
 * twice by >> 4 and >> 8), the meaning of +0x7A, and what the consumer
 * does with the two angle outputs.
 *
 * @par Callers
 * func_8003AA94 (0x8003AEBC) and func_8004B5F8 (0x8004BDE4); both
 * analysis/edges.json kind "direct".
 */
void func_8004B310(Obj4B310 *arg0, Vec4B310 *arg1, Vec4B310 *arg2, s32 arg3,
                   s16 *arg4, s16 *arg5, s32 *arg6, Vec4B310 *arg7)
{
    s32 s0;

    s0 = arg0->unk78 - arg0->unk76;
    if (s0 > 0) {
        *arg4 = arg0->unk76 + (((u32)(s0 * (func_80019668() & 0xFFF))) >> 12);
        *arg5 = arg0->unk76 + (((u32)(s0 * (func_80019668() & 0xFFF))) >> 12);
    } else {
        *arg4 = arg0->unk78;
        *arg5 = arg0->unk78;
    }

    *arg6 = ((func_80019668() & 0x1F) + 0xA0) << 3;
    *arg6 = -((*arg6 * arg0->unk7C) / 100) * 16;

    func_8001A180(arg1);

    s0 = arg0->unk68 - arg0->unk66;
    if (s0 > 0) {
        s0 += arg0->unk66 + (((u32)(s0 * (func_80019668() & 0xFFF))) >> 12);
    } else {
        s0 = arg0->unk68;
    }

    arg7->x = ((arg1->x >> 4) * s0) >> 8;
    arg7->y = ((arg1->y >> 4) * s0) >> 8;
    arg7->z = ((arg1->z >> 4) * s0) >> 8;

    if (arg7->y < 0) {
        arg7->y = -arg7->y;
    }

    s0 = arg0->unk6C - arg0->unk6A;
    if (s0 > 0) {
        s0 = arg0->unk6A + (((u32)(s0 * (func_80019668() & 0xFFF))) >> 12);
    } else {
        s0 = arg0->unk6C;
    }

    if (s0 != 0) {
        arg7->x += (arg0->unk70 * s0) >> 8;
        arg7->y += (arg0->unk72 * s0) >> 8;
        arg7->z += (arg0->unk74 * s0) >> 8;
    }

    if (arg3 & 0xFF) {
        arg7->x += arg2->x;
        arg7->y += arg2->y;
        arg7->z += arg2->z;
    }

    if (arg0->unk7B & 4) {
        arg7->y = 0;
    }
}
