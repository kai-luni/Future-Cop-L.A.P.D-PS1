// Categories: Physics > Motion | Gameplay > Object Dynamics | Graphics > Scene Graph
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct UnkAlloc {
    s32 unk00;
    u16 unk04;
    u16 unk06;
    s16 unk08;
    s16 unk0A;
    struct Vec3s32 unk0C;
    s16 unk18;
};

struct UnkSub {
    u8 pad00[4];
    s32 unk04[3];
    u8 pad10[0x18];
    struct Vec3s32 pos;
};

struct UnkInner {
    u8 pad00[0x34];
    struct UnkSub *unk34;
    u8 pad38;
    u8 unk39;
};

struct UnkObject {
    u8 pad00[0x0F];
    u8 flags0F;
    u8 pad10[0x0C];
    struct UnkInner *unk1C;
    u8 pad20[0x14];
    s32 unk34[3];
    u8 pad40[0x40];
    struct UnkAlloc *unk80;
};

extern void func_800188D0(s16 a, s16 b, s32 c, void *out);
extern void func_80010820(void *p);
extern void func_8001006C(void *dst, void *src, s32 n);
extern void gVectorAddScaled(struct Vec3s32 *a, struct Vec3s32 *b, s32 scale, struct Vec3s32 *out) __asm__("func_8001ACA4");
extern s32 func_80066F78(struct Vec3s32 *pos, s32 a1, s32 a2, s32 a3);

/**
 * @brief Advances a launched object one step: rotates its frame, moves
 *        the attached record along the velocity, spins the angles and
 *        checks the ground.
 *
 * @param object Object record; unk80 is the launch parameter block,
 *               unk1C the optional attachment.
 * @param arg Time step.
 * @return Nothing.
 *
 * @par Confirmed
 * The attachment sub-record is used only when unk1C != 0 and unk39 & 2;
 * otherwise flags0F |= 1 and the function returns. func_800188D0 builds
 * the rotation from unk80->unk04/unk06, func_80010820 loads it and
 * func_8001006C transforms the three vectors at object->unk34 into
 * sub->unk04. With vec = (0, unk80->unk00, 0) and scale = arg * 218,
 * gVectorAddScaled accumulates vec * scale into unk80->unk0C and then
 * unk80->unk0C * scale into sub->pos. The angles advance by
 * (unk08 * arg) >> 8 and (unk0A * arg) >> 8. When
 * func_80066F78(&sub->pos, 0, 0, 0) >= sub->pos.y, unk18 = -1 (ground
 * reached); otherwise a positive unk18 counts down by arg.
 *
 * @par Likely role
 * Ballistic or thrown object: unk00 the vertical speed, unk0C the
 * integrated position, unk18 a fuse that expires on impact.
 *
 * @par Callers
 * func_8004C228.
 */
void func_8004AFC0(struct UnkObject *object, s32 arg)
{
    struct UnkSub *sub;
    struct Vec3s32 vec;
    s32 mat[9];
    s32 scale;

    sub = 0;
    if (object->unk1C != 0 && (object->unk1C->unk39 & 2)) {
        sub = object->unk1C->unk34;
    }

    if (sub == 0) {
        object->flags0F |= 1;
        return;
    }

    func_800188D0(object->unk80->unk04, object->unk80->unk06, 0, mat);
    func_80010820(mat);
    func_8001006C(object->unk34, sub->unk04, 3);

    vec.x = 0;
    vec.y = object->unk80->unk00;
    vec.z = 0;

    scale = arg * 218;

    gVectorAddScaled(&object->unk80->unk0C, &vec, scale, &object->unk80->unk0C);
    gVectorAddScaled(&sub->pos, &object->unk80->unk0C, scale, &sub->pos);

    object->unk80->unk04 += (object->unk80->unk08 * arg) >> 8;
    object->unk80->unk06 += (object->unk80->unk0A * arg) >> 8;

    if (func_80066F78(&sub->pos, 0, 0, 0) >= sub->pos.y) {
        object->unk80->unk18 = -1;
        return;
    }

    if ((s16)object->unk80->unk18 > 0) {
        object->unk80->unk18 -= arg;
    }
}
