// Categories: Graphics > Effects > Particle Update | Game Engine > Entity Lifecycle | Graphics > Effects > Sprite Animation
#include "game/types.h"

typedef struct Obj Obj;
typedef struct Sub { u8 pad[0x2e]; u16 field2e; } Sub;
struct Obj {
    Obj *next;
    u8 pad04[0xb];
    u8 flags;
    u8 pad10[2];
    s16 field12;
    u8 pad14[0xc];
    Sub *sub;
    u8 pad24[0x10];
    u8 matrix[0x34];
    s16 field68, field6a, field6c, field6e, field70;
    s16 field72;
    s16 field74, field76, field78, field7a, field7c, field7e;
    s16 field80, field82, field84, field86;
    u8 pad88;
    u8 c89, c8a, c8b, pad8c, c8d, c8e, c8f;
    s16 field90, field92, field94, field96;
    u16 field98;
    s16 field9a;
};
extern s32 gSceneFrameCounter __asm__("D_8009FA50");
extern void gDrawGroundMarker(Obj *) __asm__("func_8006D520");
extern void gBuildRotationMatrix(s32, s32, s32, void *) __asm__("func_800188D0");
void gTickEffectNodes(Obj *node, s32 dt) __asm__("func_8006D770");


/**
 * @brief Per-frame update of a list of effect nodes: colour fade, sprite-sheet
 *        frame advance, rotation matrix, scale decay, lifetime and the
 *        re-emission call.
 *
 * @param node Head of the singly linked list (next at +0); 0 ends the call.
 * @param dt Frame delta in 1/256 units, the scale of every rate below.
 * @return Nothing.
 *
 * @par Confirmed
 * Nodes with bit 3 of the +0x0C flag byte are skipped. While bit 0 of +0x98 is
 * clear and the rate +0x86 is non-zero, amount = (+0x86 * dt) >> 8 is
 * subtracted from the six bytes +0x89, +0x8A, +0x8B, +0x8D, +0x8E and +0x8F -
 * two RGB triples - each saturating at 0 and collecting a bit in a six-bit
 * mask; when the mask is full the sub-block's +0x2E gate is tested and
 * func_8006D520 is called, then +0x0C gains bit 0. The frame timer +0x76 is
 * reduced by dt; when it reaches zero the overshoot is negated, the frame
 * index +0x72 advances by 1 plus the
 * overshoot divided by 20, is wrapped modulo the frame count +0x74, the
 * column is taken modulo the column count +0x84, the timer is re-armed as
 * 20 - overshoot modulo 20, and the two atlas offsets are rebuilt as
 * +0x7C = +0x78 + column * +0x80 and +0x7E = +0x7A + row * +0x82. The counter
 * +0x12 advances by (+0x68 * dt) >> 8. When the rotation rates +0x94/+0x96 are
 * non-zero the angles +0x90/+0x92 advance modulo 4096 and func_800188D0(+0x90,
 * 1024 - +0x92, 0, +0x14) rebuilds the 3x3 matrix at +0x14. When +0x6A is
 * positive the rate +0x68 is lowered by (+0x6A * dt) >> 7 and floored at 4096,
 * with +0x6A cleared. With the sub-block gate set and (D_8009FA50 & 3) equal to 3
 * func_8006D520 is called every fourth frame. Finally, when +0x12 reaches the
 * limit +0x6E, one last func_8006D520 is made if the sub-block gate is set
 * and +0x9A differs from +0x12, and +0x0C gains bit 0.
 *
 * @par Likely role
 * The shared tick of the scene's sprite effects: the six colour bytes are the
 * fading tint, +0x76/+0x74/+0x84 describe a sprite-sheet row animation, +0x68
 * with +0x6A a growing or shrinking billboard and +0x90/+0x92 a spin, so a
 * single routine drives every particle that is a textured quad. func_8006D520
 * resolves the node transform and emits its current appearance as a separate
 * record, which the fourth-frame and terminal calls turn into a trail and a
 * final puff.
 *
 * @par Unknown
 * The meaning of +0x98 bit 0 (a freeze?), of the +0x2E gate on the sub-block,
 * and why the emission wants exactly every fourth frame.
 *
 * @par Callers
 * func_8003F4A0, the second of its six list passes, over D_800A1070[2].
 *
 * @par Environment path
 * Per-frame update of the effect objects in gListHeads bucket 2;
 * gRunSceneListPasses (func_8003F4A0) calls it with the frame delta as the
 * first of its list passes, inside the entity tick and not in the render
 * path. For every live node it fades the six colour bytes at +0x89..+0x8F and
 * retires the node when all reach zero, advances the sprite-sheet frame and
 * its texture offsets from the timers at +0x72..+0x84, grows the value at
 * +0x12 by the rate at +0x68 (which decays towards 4096 by +0x6A), and
 * rebuilds the spin matrix with func_800188D0 when spin rates are set. When
 * the type's +0x2E field is set it calls func_8006D520 every fourth frame
 * (gSceneFrameCounter & 3) and once more at the end of the node's life, when
 * +0x12 reaches +0x6E and the node is retired.
 */
void gTickEffectNodes(Obj *node, s32 dt)
{
    Obj *p;
    s32 amount;
    u8 mask;
    s32 timer;
    s16 rem;
    s32 col;
    s32 x, y;
    s32 frame;
    for (; node; node = node->next) {
        if (node->flags & 8) continue;
        p = node;
        if (!p) continue;
        if (!(p->field98 & 1) && p->field86 != 0) {
            amount = (p->field86 * dt) >> 8;
            mask = 0;
            if (p->c89 < amount) { mask |= 1; p->c89 = 0; } else p->c89 -= amount;
            if (p->c8a < amount) { mask |= 2; p->c8a = 0; } else p->c8a -= amount;
            if (p->c8b < amount) { mask |= 4; p->c8b = 0; } else p->c8b -= amount;
            if (p->c8d < amount) { mask |= 8; p->c8d = 0; } else p->c8d -= amount;
            if (p->c8e < amount) { mask |= 16; p->c8e = 0; } else p->c8e -= amount;
            if (p->c8f < amount) { mask |= 32; p->c8f = 0; } else p->c8f -= amount;
            if (mask == 63) {
                if (p->sub->field2e) gDrawGroundMarker(p);
                p->flags |= 1;
            }
        }
        timer = (u16)p->field76 - dt;
        p->field76 = timer;
        if ((s16)timer <= 0) {
            p->field76 = -p->field76;
            *(volatile s16 *)&p->field72 += 1 + p->field76 / 20;
            frame = p->field72;
            rem = frame % p->field74;
            *(volatile s16 *)&p->field72 = rem;
            col = rem % p->field84;
            p->field76 = 20 - p->field76 % 20;
            x = p->field72 / p->field84;
            p->field7c = p->field78 + col * p->field80;
            p->field7e = p->field7a + x * p->field82;
        }
        p->field12 += (p->field68 * dt) >> 8;
        if (*(s32 *)&p->field94) {
            p->field90 = (p->field90 + (s16)((dt * p->field94) >> 8)) % 4096;
            p->field92 = (p->field92 + (s16)((dt * p->field96) >> 8)) % 4096;
            gBuildRotationMatrix(p->field90, 1024 - p->field92, 0, p->matrix);
        }
        if (p->field6a > 0) {
            p->field68 -= (p->field6a * dt) >> 7;
            if (p->field68 < 4096) { p->field68 = 4096; p->field6a = 0; }
        }
        if (p->sub->field2e && (gSceneFrameCounter & 3) == 3) gDrawGroundMarker(p);
        if (p->field12 >= p->field6e) {
            if (p->sub->field2e && p->field9a != p->field12) gDrawGroundMarker(p);
            p->flags |= 1;
        }
    }
}
