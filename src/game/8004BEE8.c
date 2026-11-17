// Categories: Physics > Motion | Gameplay > Object Dynamics
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct OtherObj {
    u8 unk00[0x28];
    struct Vec3s32 pos;
};

struct SubNode {
    u8 unk00[0x28];
    u8 unk28;
    u8 unk29[3];
    u16 unk2C;
    u16 unk2E;
    s16 unk30;
    s16 unk32;
    struct Vec3s32 vel;
    s32 unk40;
    s16 unk44;
    u8 unk46[2];
    struct SubNode *next;
    union {
        struct OtherObj *obj;
        struct Vec3s32 pos;
    } u;
};

struct Node4BEE8 {
    u8 unk00[0x0F];
    u8 flags0F;
    u8 unk10[0x74];
    struct SubNode *unk84;
};

extern s32 gSceneFrameCounter __asm__("D_8009FA50");
extern void func_8001ACA4(struct Vec3s32 *a, struct Vec3s32 *b, s32 scale, struct Vec3s32 *out);
extern s32 func_80066F78(struct Vec3s32 *position, s32 arg1, s32 arg2, s32 arg3);

/**
 * @brief Integrates the debris/shard list of a fractured object: gravity,
 *        spin, ground contact and lifetime.
 *
 * @param node Owner; +0x84 the shard list head, +0xF the done flag byte.
 * @param arg Frame delta.
 * @return Nothing.
 *
 * @par Confirmed
 * scale = arg * 218. An empty list sets flag bit 0 and returns. Per shard:
 * vel.y += unk40 * scale (64-bit multiply, >> 16), position integrated via
 * func_8001ACA4 (the +0x80 flag selects the attached object's position
 * over the local one), spin words += (unk30/unk32 * arg) >> 8. Every 8th
 * shard ((frame ^ i) & 7 == 0, frame = D_8009FA50) a ground test
 * func_80066F78(pos, 0, 0, 0) >= pos.y kills it (unk44 = -1); a positive
 * unk44 counts down by arg and kills at 0.
 *
 * @par Likely role
 * The fracture-shard physics behind func_8004C228 (the breakable-object
 * handler): 218 is the gravity constant per frame unit, the staggered
 * ground test spreads the expensive height queries over frames.
 *
 * @par Unknown
 * What unk40 encodes per shard (likely a gravity scale).
 *
 * @par Callers
 * func_8004C228.
 */
void func_8004BEE8(struct Node4BEE8 *node, s32 arg_)
{
    register s32 arg __asm__("$18") = arg_;
    register s32 neg1 __asm__("$20");
    register s32 frame __asm__("$21");
    register struct SubNode *cur __asm__("$16");
    register s32 i __asm__("$17");
    register u32 wide0 __asm__("$8");
    register u32 wide1 __asm__("$9");
    register s32 mul __asm__("$4");
    register s32 val __asm__("$2");
    s32 scale;

    scale = arg * 218;
    cur = node->unk84;
    if (cur == 0) {
        node->flags0F |= 1;
        return;
    }

    __asm__ volatile("lui %0, %%hi(D_8009FA50)\n\tlw %0, %%lo(D_8009FA50)(%0)" : "=r"(frame));
    i = 0;
    neg1 = -1;
    do {
        val = cur->unk40;
        __asm__ volatile("mult %2, %3\n\t"
                         "mflo %0\n\t"
                         "mfhi %1"
                         : "=r"(wide0), "=r"(wide1)
                         : "r"(val), "r"(scale));
        wide1 <<= 16;
        wide0 >>= 16;
        mul = wide1 | wide0;
        cur->vel.y += mul;

        if (cur->unk28 & 0x80) {
            func_8001ACA4(&cur->u.obj->pos, &cur->vel, scale, &cur->u.obj->pos);
        } else {
            func_8001ACA4(&cur->u.pos, &cur->vel, scale, &cur->u.pos);
        }

        cur->unk2C += (s16)((cur->unk30 * arg) >> 8);
        cur->unk2E += (s16)((cur->unk32 * arg) >> 8);

        if (!((frame ^ i) & 7)) {
            if (cur->unk28 & 0x80) {
                if (func_80066F78(&cur->u.obj->pos, 0, 0, 0) >= cur->u.obj->pos.y) {
                    cur->unk44 = neg1;
                }
            } else {
                if (func_80066F78(&cur->u.pos, 0, 0, 0) >= cur->u.pos.y) {
                    cur->unk44 = neg1;
                }
            }
        }

        if (cur->unk44 > 0) {
            cur->unk44 -= arg;
            if (cur->unk44 <= 0) {
                cur->unk44 = neg1;
            }
        }

        cur = cur->next;
        i++;
    } while (cur != 0);
}
