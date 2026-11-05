#include "game/types.h"

struct Unk378BCState {
    u8 unk00[0x10];
    u8 *animData;
    u8 unk14[0x20];
    u8 *frames;
};

struct Unk378BCPart {
    u8 unk00[0x24];
    u8 animIn[0x10];
    u8 output[0x14];
    u8 unk48;
    u8 unk49;
    u8 unk4A;
};

struct Unk378BCInfo {
    u8 unk00[0x18];
    s32 value;
};

struct Unk378BCOther {
    u8 unk00[0x04];
    struct Unk378BCInfo *info;
    struct Unk378BCPart *part;
};

struct Unk378BC {
    u8 unk00[0x16];
    u8 unk16;
    u8 unk17[0x11];
    struct Unk378BCState *state;
    u8 matrix[0x24];
    u8 unk50[0x0C];
    u8 unk5C[0x14];
    s16 frame;
    u8 unk72[0x2E];
    u8 unkA0[0x28];
    u8 unkC8[0x14];
    u8 unkDC[0x3C];
    u32 flags;
    u8 unk11C[0x3C];
    u8 unk158;
    u8 unk159;
    u8 unk15A;
};

extern s32 func_8002AD58(s32 message, void *object, void *data);
extern void func_80060668(void *in, void *out);
extern void func_80050E0C(void *a, void *b, s32 c, void *d, void *e);
extern void func_80037EB4(void *self);
extern void func_80044988(void *data, void *track, u8 forward, s32 index);
extern void func_80010750(void *matrix);
extern void func_80010234(void *frame, void *output, s32 count);
extern s32 func_8006314C(void *key, u8 value);

void func_800378BC(s32 message, struct Unk378BC *self, struct Unk378BCOther *other)
{
    struct Unk378BCPart *part;
    s32 index;
    register u8 *pointer __asm__("$2");
    register s32 offset __asm__("$4");

    func_8002AD58(message, self, other);
    part = other->part;
    func_80060668(part->animIn, self->unkA0);
    func_80050E0C(part->output, self->unkDC, other->info->value, self, self->unk50);
    func_80037EB4(self);
    self->unk15A = 0;
    if (self->state->animData != 0) {
        index = 0;
        if (self->unk16 == 5) {
            self->unk158 = part->unk4A;
            self->unk159 = part->unk49 | 1;
            index = part->unk48;
        }
        func_80044988(self->state->animData, self->unkC8, 1, index);
    }
    func_80010750(self->matrix);
    offset = self->frame;
    pointer = (u8 *)self->state;
    offset <<= 4;
    __asm__("" : "=r"(offset) : "0"(offset));
    pointer = ((struct Unk378BCState *)pointer)->frames;
    offset += 8;
    func_80010234(pointer + offset, self->unk5C, 1);
    if (self->flags & 0x4000) {
        func_8006314C(self, 0);
    }
}
