#include "game/types.h"

struct Data { u8 pad[0x24]; s8 count; u8 end; s16 pos[3]; s16 speed[3]; s16 other[2]; s16 angle; u8 flags; u8 kind; u8 extra; };
struct Arg { u8 pad[8]; struct Data *data; };
struct Model { u8 pad[0x34]; u8 *data; };
struct Self { u8 pad[0x21]; u8 old; u8 pad22[6]; struct Model *model; u8 basis[0x24]; s32 pos[3]; u8 rest[0x14]; s16 index; u8 pad72[0x2e]; s32 values[3]; s32 speeds[3]; s32 other[2]; s32 current; s16 zero; u8 flags; u8 kind; s8 count; s8 last; s8 end; s8 mode; u8 previous; volatile u8 extra; };
extern void func_8002AD58(s32, struct Self *, struct Arg *, s32);
extern void func_8006314C(struct Self *, u8);
extern void func_800188D0(s32, s32, s32, void *);
extern s32 func_80066F78(void *, s32, s32, s32);
extern void func_80010750(void *);
extern void func_80010234(void *, void *, s32);
void func_80041488(struct Self *self, struct Arg *arg, s32 a2)
{
    struct Data *d;
    s32 i, base, offset;
    register s32 end __asm__("$3"); register s32 old __asm__("$2");
    register s32 count __asm__("$4");
    register s32 last __asm__("$3");
    func_8002AD58(2, self, arg, a2);
    d = arg->data;
    self->count = d->count;
    self->values[0] = d->pos[0] << 4;
    self->values[1] = d->pos[1] << 4;
    self->values[2] = d->pos[2] << 4;
    self->speeds[0] = (d->speed[0] << 8) / 218;
    self->speeds[1] = (d->speed[1] << 8) / 218;
    self->speeds[2] = (d->speed[2] << 8) / 218;
    self->other[0] = (d->other[0] << 4) / 300;
    self->other[1] = (d->other[1] << 4) / 300;
    self->mode = 1;
    count = self->count;
    self->flags = d->flags;
    self->kind = d->kind;
    self->extra = d->extra;
    old = *(volatile u8 *)&self->old;
    end = *(volatile u8 *)&d->end;
    *(volatile u8 *)&self->old = 0;
    *(volatile u8 *)&self->previous = old;
    *(volatile s8 *)&self->end = end;
    last = end - 1;
    self->last = last;
    if (self->end >= count) {
        register s32 tmp __asm__("$2");
        tmp = last;
        __asm__("" : "+r"(tmp));
        self->mode = 2;
        self->end = tmp - 1;
    }
    if (self->flags & 5) self->current = 1;
    else self->current = self->speeds[self->last];
    self->zero = 0;
    func_8006314C(self, d->kind);
    func_800188D0(d->angle, 0, 0, self->basis);
    base = func_80066F78(self->pos, 0, 0, 0);
    for (i = 0; i < self->count; i++) self->values[i] += base;
    self->pos[1] = self->values[self->last];
    func_80010750(self->basis);
    offset = (self->index << 4) + 8;
    func_80010234(self->model->data + offset, (u8 *)self + 0x5c, 1);
}
