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

extern struct Global *D_8009FA94;
extern s32 func_80019668(void);
extern void func_80033608(struct Entity *self, void *anim);
extern void func_8004D958(struct State *state);
extern struct State *func_80035184(struct Entity *self, s32 *out);
extern void func_8003526C(struct Entity *self);
extern void func_800336DC(struct Entity *self);
extern void func_80060238(void *arg0, s32 arg1, struct Entity *arg2, s32 arg3, s32 arg4);
extern void func_800324E0(void *arg0, struct Entity *self, s32 arg2);

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

            self->unkF4 += self->unkF4 >> (4 - (D_8009FA94->unk4C >> 2));

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
        dist = (9 - D_8009FA94->unk4C) * 80 + 128;
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
