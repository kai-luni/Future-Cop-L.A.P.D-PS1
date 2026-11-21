// Categories: Physics > Vertical Movement | Gameplay > Movement
#include "game/types.h"
struct Obj { u8 pad0[6]; s16 y; u8 pad8[4]; s16 scale; u8 pade[14]; u32 value; s16 divisor; u8 pad22[4]; s16 offset; u8 pad28[8]; s16 speed; s16 current; u8 pad34[8]; u32 flags; s16 timer; };
struct Entry { u8 pad0[2]; s16 a; u8 pad4[4]; s16 b; u8 padA[6]; };
struct Child { u8 pad0[0x34]; struct Entry *entries; };
struct State { u8 pad0[0x28]; struct Child *child; u8 pad2c[0x24]; u8 pos[4]; s32 height; u8 pad58[0x18]; s16 index; u8 pad72[0x12]; s32 changed; };
extern s32 gQuerySurfaceHeight(void *, s32) __asm__("func_80067684");

/**
 * @brief Moves an object vertically towards a target height.
 *
 * Two flag combinations are handled. With (flags & 0x50) == 0x10 the target is
 * computed from the object's own fields: a reciprocal (0x80000000 / (divisor
 * << 7)) multiplied by the value word, then multiplied by the scale and shifted
 * back into 16.16, plus the offset and the y bias. A height below the target
 * climbs with (speed * delta) >> 1 and stops at the target; on arrival the
 * object loses flag 0x10, the timer is cleared and 0x18 records the speed. With
 * (flags & 0x50) == 0x40 the target is the ground height from func_80067684
 * plus the terrain offset of the current entry; the object descends towards it,
 * and on arrival flag 0x10 is set again. Both branches report the new height
 * through state->height and set state->changed.
 *
 * @param obj Object with the motion fields (scale, divisor, value, offset, speed).
 * @param delta Frame delta.
 * @param state State with the current height, the ground position and the entry index.
 * @return 1 when the object moved or arrived, 0 when no mode matched.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Two mode blocks with a shared store path.
 *
 * @par Calls
 * func_80067684 - ground height query.
 *
 * @par Side effects
 * Writes the object speed/timer/flags and the state height.
 *
 * @par Callers
 * Confirmed: func_80050D14.
 */
s32 func_80050B54(struct Obj *obj, s32 delta, struct State *state)
{
    register u32 lo __asm__("$8");
    register u32 hi __asm__("$9");
    register u32 recip __asm__("$3");
    register s32 divisor __asm__("$2");
    register u32 value __asm__("$4");
    register u32 product __asm__("$3");
    register s32 scale __asm__("$2");
    register s32 target __asm__("$4");
    s32 height;
    u32 flags = obj->flags;
    if ((flags & 0x50) == 0x10) {
        divisor = obj->divisor;
        height = state->height;
        value = obj->value;
        recip = 0x80000000u / (divisor << 7);
        scale = obj->scale;
        __asm__ volatile("" : "+r"(scale));
        __asm__ volatile("nop
	multu %2, %3\n\tmflo %0\n\tmfhi %1" : "=r"(lo), "=r"(hi) : "r"(value), "r"(recip));
        hi <<= 16;
        lo >>= 16;
        __asm__ volatile("" : "+r"(lo), "+r"(hi));
        product = hi | lo;
        __asm__ volatile("mult %2, %3\n\tmflo %0\n\tmfhi %1" : "=r"(lo), "=r"(hi) : "r"(scale << 8), "r"(product));
        hi <<= 16;
        lo >>= 16;
        __asm__ volatile("" : "+r"(lo), "+r"(hi));
        target = hi | lo;
        target = (obj->offset << 4) + (obj->y << 8) + target;
        if (height < target) {
            height += (obj->speed * delta) >> 1;
            {
                register s32 compare __asm__("$2") = height < target;
                __asm__ volatile("" : "+r"(compare));
                if (compare) goto store;
            }
        }
        obj->flags = flags & ~0x10;
        obj->timer = 0;
        obj->current = obj->speed;
        state->changed = 1;
        goto success;
    }
    if ((flags & 0x50) == 0x40) {
        height = state->height;
        target = gQuerySurfaceHeight(state->pos, -1);
        if (state->child != 0) {
            register s32 offset __asm__("$2") = state->index;
            register struct Entry *base __asm__("$3") = state->child->entries;
            register struct Entry *e __asm__("$3");
            offset <<= 4;
            __asm__ volatile("" : "+r"(offset));
            offset += 8;
            __asm__ volatile("" : "+r"(offset));
            e = (struct Entry *)((u8 *)base + offset);
            target += (e->b - e->a) << 4;
        }
        if (target < height) {
            height -= (obj->speed * delta) >> 1;
            if (target < height) goto store;
        }
        height = target;
        obj->flags |= 0x10;
store:
        state->height = height;
        goto success;
    }
    return 0;
success:
    return 1;
}
