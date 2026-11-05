#include "game/types.h"

struct Vec { s32 x, y, z; };
struct State { u8 pad[0x34]; u8 *frames; };
struct Object {
    u8 pad00[0x0c];
    u8 command;
    u8 pad0d[0x17];
    u32 flags;
    struct State *state;
    u8 matrix[0x24];
    struct Vec position;
    struct Vec output;
    u8 pad68[8];
    s16 frame;
    u8 pad72[0x2e];
    u8 anim[0x3c];
    u32 animflags;
    u8 pade0[0x3c];
    u8 bytes[4];
};
struct Part { u8 pad[0x24]; u8 data[0x14]; u8 bytes[4]; };
struct Info { u8 pad[0x18]; s32 value; };
struct Other { u8 pad[4]; struct Info *info; struct Part *part; };
extern s32 func_8002AD58(s32, struct Object *, struct Other *);
extern void func_80050E0C(void *, void *, s32, void *, void *);
extern s32 func_80050D14(void *, struct Other *, struct Object *);
extern void func_80039BD0(struct Object *);
extern void func_80010750(void *);
extern void func_80010234(void *, void *, s32);
extern struct Object *D_8009F9A0;
#define current D_8009F9A0
#define barrier() __asm__ volatile("" : : : "memory")

s32 func_80039C58(s32 message, struct Object *self, struct Other *other)
{
    struct Part *part;
    register s32 offset __asm__("$4");
    register u8 *pointer __asm__("$2");
    register s32 result __asm__("$2");
    switch (message) {
    case 1:
        return 0x120;
    case 2:
        func_8002AD58(message, self, other);
        part = other->part;
        self->bytes[0] = part->bytes[0];
        self->bytes[1] = part->bytes[1];
        self->bytes[2] = part->bytes[2];
        self->bytes[3] = part->bytes[3];
        func_80050E0C(part->data, self->anim, other->info->value, self, &self->position);
        self->flags &= 0x7fffffff;
        if (self->state) {
            func_80010750(self->matrix);
            offset = self->frame;
            pointer = (u8 *)self->state;
            offset <<= 4;
            __asm__("" : "=r"(offset) : "0"(offset));
            pointer = ((struct State *)pointer)->frames;
            offset += 8;
            func_80010234(pointer + offset, &self->output, 1);
        } else {
            self->output = self->position;
        }
    set_current:
        if (!(self->animflags & 0x40)) { current = self; barrier(); }
        return 0;
    case 6:
        if (self->command) func_80039BD0(self);
        if (func_80050D14(self->anim, other, self)) {
            if (self->state) {
                func_80010750(self->matrix);
                offset = self->frame;
            pointer = (u8 *)self->state;
            offset <<= 4;
            __asm__("" : "=r"(offset) : "0"(offset));
            pointer = ((struct State *)pointer)->frames;
            offset += 8;
            func_80010234(pointer + offset, &self->output, 1);
            } else {
                self->output = self->position;
            }
        }
        if (current != self) goto set_current;
        result = self->animflags & 0x40;
        if (!result) goto zero_return;
        result = 0;
        current = 0;
        __asm__ volatile("" : "+r"(result) : : "memory");
        return result;
    case 5:
        if (current == self) { current = 0; barrier(); }
        return func_8002AD58(message, self, other);
    case 3: case 4: case 10: case 11: case 13: case 14:
    zero_return:
        barrier();
        return 0;
    default:
        return func_8002AD58(message, self, other);
    }
}
