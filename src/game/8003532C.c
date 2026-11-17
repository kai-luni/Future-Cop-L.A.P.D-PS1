// Categories: Gameplay > AI > Drone Attachment | Gameplay > Entity Update | Game Engine > Entity Lifecycle
#include "game/types.h"

typedef struct {
    s32 x, y, z;
} Vec3;

struct State {
    u8 pad00[0x0C];
    Vec3 pos;
    s16 unk18;
    u8 unk1A;
    u8 unk1B;
    s16 unk1C;
    s16 unk1E;
    u16 unk20;
    u8 pad22[0x2E];
    Vec3 unk50;
    u8 pad5C[0x48];
    s32 unkA4;
    u8 padA8[0x20];
    s32 unkC8;
};

struct Entity {
    u8 pad00[0x1C];
    s16 unk1C;
    u8 pad1E[0x32];
    s32 unk50;
    u8 pad54[4];
    s32 unk58;
    u8 pad5C[0x80];
    Vec3 pos;
    u8 padE8[0x0C];
    s32 unkF4;
    s16 unkF8;
    u8 unkFA;
    u8 padFB[0x39];
    struct State *unk134;
};

struct Global {
    u8 pad[0x4C];
    s32 unk4C;
};

extern struct Global *gGameSession __asm__("D_8009FA94");
extern s32 func_80019668(void);
extern void func_80033608(struct Entity *self, void *anim);
extern void func_8004D958(struct State *state);
extern struct State *func_80035184(struct Entity *self, s32 *out);
extern void func_8003526C(struct Entity *self);
extern void func_800336DC(struct Entity *self);
extern void func_80060238(void *arg0, s32 arg1, struct Entity *arg2, s32 arg3, s32 arg4);
extern void func_800324E0(void *arg0, struct Entity *self, s32 arg2);

/**
 * @brief Message-6 tick of the drone class: runs the two-stage approach to a
 *        strategic node, re-arms the AI re-think timer, ticks the two attack
 *        descriptors and hands the frame to the flyer mover.
 *
 * @param message Message id, forwarded unchanged to func_800324E0.
 * @param self Entity block: +0x1C approach word, position +0x50/+0x54/+0x58,
 *        position copy +0xDC, +0xF4 charge word, +0xF8 cached value, +0xFA state
 *        byte, +0x134 pointer to the AI record.
 * @param delta Frame time step, subtracted from the AI timer and forwarded to both
 *        attack descriptors and to the mover.
 * @return Nothing.
 *
 * @par Confirmed
 * The entity word +0x1C is copied into the AI record at +0xA4. The AI mode byte is
 * at +0x1A and the limit it selects is +0x1E, halved for modes 1 and 2.
 * While +0x1C is below that limit the approach runs: the planar octagonal distance
 * between the entity position and the AI record position at +0x0C is measured as
 * |dx| + |dz| - min / 2, and below 0x4000 the entity is reset with
 * func_8003526C(self) and the timer is run instead.
 * State 0: the entity position takes the AI record vector at +0x0C, the state
 * becomes 1, +0xF4 grows by +0xF4 >> (4 - (D_8009FA94->+0x4C >> 2)),
 * func_80033608(self, 0) parks the animation and the tail runs.
 * State 1: func_80035184(self, &distance) returns the nearest live object of type
 * 16 whose flag word has bit 0x20000 set. With a hit, a distance at or below
 * 0x13FFF promotes the cached +0xF8 from the AI value +0x20 to +0x20 << 2 while the
 * two are still equal; below 0x1000 the object is torn down with func_8004D958 when
 * its +0xC8 has bit 0x02000000, and the entity is reset as above; closer than the
 * AI distance the entity position is copied from the object's +0x50. With no hit
 * the same +0xF8 promotion runs for a distance at or below 0x13FFF and the position
 * is copied from the AI record at +0x0C.
 * Any other state skips the approach and only runs the timer.
 * Timer: the AI word +0x18 loses delta; when it goes negative it reloads to
 * (9 - D_8009FA94->+0x4C) * 80 + 128, plus (random & 0xFF) or, for mode 2, minus
 * (random & 0x1F), and func_800336DC(self) runs the strategic tick.
 * Tail, reached from every path: func_80060238(ai + 0x50, delta, self, 0, 1) and
 * func_80060238(ai + 0x78, delta, self, 0, 1) tick the two attack descriptors held
 * in the AI record, then func_800324E0(message, self, delta) runs the shared flyer
 * tick.
 *
 * @par Likely role
 * An AI-controlled drone that flies to a strategic node of its own side and latches
 * onto it: states 0 and 1 are "acquire" and "track", and the states set by the
 * spawn half func_80035698 (4, and 5 which arms the countdown at +0x100 in the
 * handler) bypass the approach. The compounding +0xF4 term reads like a charge or
 * growth rate that the difficulty word D_8009FA94->+0x4C accelerates, and the
 * re-think period of 80 frames per difficulty step matches the strategic AI it
 * wakes up.
 *
 * @par Unknown
 * - What +0x1C and the AI word +0x1E are; the pair is written identically by the
 *   spawn half, so an altitude band and a period are both plausible.
 * - Why the AI record carries two attack descriptors at +0x50 and +0x78.
 * - What +0xF8 selects, and whether the "promote to << 2" is a size or a damage
 *   change.
 *
 * @par Callers
 * func_800357C0, message 6 of its dispatch table; the same table answers 0x138 for
 * message 1 (the block size) and calls func_80035698 for message 2.
 */
void func_8003532C(void *arg0, struct Entity *self, s32 arg2) {
    struct State *a2;
    struct State *p;
    s32 local18;
    s32 dist;
    
    
    register s32 other __asm__("$4");
    register s32 limit __asm__("$5");
    register s32 v0 __asm__("$2");
    register s32 v1 __asm__("$3");

    s32 sign;
    s32 mode;

    self->unk134->unkA4 = self->unk1C;

    mode = self->unk134->unk1A;
    if (mode < 3 && mode != 0) {
        __asm__("" : "=r"(v0) : "0"(self->unk134->unk1E)); limit = (s16)v0 >> 1;
    } else {
        limit = self->unk134->unk1E;
    }

    if (self->unk1C < limit) {
        a2 = self->unk134;
        v1 = self->unk50;
        v0 = a2->pos.x;
        dist = v1 - v0;
        v1 = dist >> 31;
        v0 = v1 + dist;
        dist = v0 ^ v1;

        v1 = self->unk58;
        v0 = a2->pos.z;
        other = v1 - v0;
        v1 = other >> 31;
        v0 = v1 + other;
        other = v0 ^ v1;

        if (other < dist) {
            limit = other;
        } else {
            limit = dist;
        }
        v0 = limit / 2;
        dist += other - v0;

        if (dist < 0x4000) {
            func_8003526C(self);
            goto L_timer;
        }

        if (self->unkFA == 0) {
            self->pos = a2->pos;
            self->unkFA = 1;

            self->unkF4 += self->unkF4 >> (4 - (gGameSession->unk4C >> 2));

            func_80033608(self, 0);
            goto L_final;
        } else if (self->unkFA == 1) {
            a2 = func_80035184(self, &local18);
            if (a2 != 0) {
                if (local18 <= 0x13FFF) {
                    if (self->unkF8 == (s16)self->unk134->unk20) {
                        self->unkF8 = self->unk134->unk20 << 2;
                    }
                }

                if (local18 < 0x1000) {
                    if (a2->unkC8 & 0x02000000) {
                        func_8004D958(a2);
                    }
                    func_8003526C(self);
                    goto L_timer;
                }

                if (local18 < dist) {
                    self->pos = a2->unk50;
                    goto L_final;
                }
                goto L_timer;
            } else {
                if (dist <= 0x13FFF) {
                    if (self->unkF8 == (s16)self->unk134->unk20) {
                        self->unkF8 = self->unk134->unk20 << 2;
                    }
                }
                p = self->unk134;
                self->pos = p->pos;
                goto L_final;
            }
        } else {
            goto L_timer;
        }
    }

L_timer:
    self->unk134->unk18 -= arg2;
    if (self->unk134->unk18 < 0) {
        dist = (9 - gGameSession->unk4C) * 80 + 128;
        if (self->unk134->unk1A != 2) {
            dist += func_80019668() & 0xFF;
        } else {
            dist -= func_80019668() & 0x1F;
        }
        self->unk134->unk18 = dist;
        func_800336DC(self);
    }

L_final:
    func_80060238((u8 *)self->unk134 + 0x50, arg2, self, 0, 1);
    func_80060238((u8 *)self->unk134 + 0x78, arg2, self, 0, 1);
    func_800324E0(arg0, self, arg2);
}
