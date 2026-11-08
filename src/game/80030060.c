#include "game/types.h"
struct Entity { u8 pad[0x5c]; s32 x; u8 pad60[4]; s32 z; u8 pad68[0x60]; u32 flags; };
extern void func_8003526C(struct Entity *);
extern void func_8004D958(struct Entity *);
void func_80030060(struct Entity *self, struct Entity *other)
{
    s32 dz, dx, ax, sum;
    dz = self->z - other->z;
    dx = self->x - other->x;
    if (dz < 0) dz = -dz;
    ax = dx;
    if (ax < 0) ax = -ax;
    if (dz < ax) { dz ^= ax; ax ^= dz; dz ^= ax; }
    sum = dz + ax;
    if ((dz >> 1) < ax) {
        sum = (sum << 6) + (sum << 3) + (sum << 1) + dz + (ax << 5);
        ax = dz - ax;
        dz = sum >> 7;
    }
    if (dz + (ax >> 2) - (ax >> 6) < 0x3000) {
        if (self->flags & 0x20000) func_8003526C(other);
        if (self->flags & 0x2000000) func_8004D958(self);
    }
}
