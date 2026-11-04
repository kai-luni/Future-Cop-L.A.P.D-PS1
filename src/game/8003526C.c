#include "game/types.h"

struct State {
    u8 pad00[0x18];
    s16 unk18;
    u8 pad1A[4];
    s16 unk1E;
    u16 unk20;
    u8 pad22[2];
    s32 unk24;
};

struct Entity {
    u8 pad00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 unk20;
    u8 pad21[3];
    u32 unk24;
    u8 pad28[0xCC];
    s32 unkF4;
    s16 unkF8;
    u8 unkFA;
    u8 padFB[0x39];
    struct State *unk134;
};

struct GlobalFA94 {
    u8 pad00[0xE0];
    s32 unkE0;
};

extern struct GlobalFA94 *D_8009FA94;

extern void func_8003BA50(struct Entity *self, s32 old1C);
extern s32 func_80019668(void);
extern void func_800330E8(struct Entity *self);

void func_8003526C(struct Entity *self)
{
    s32 old1C = self->unk1C;
    self->unk1C = self->unk1E;
    if (self->unk20 != 0 && !(self->unk24 & 0x40000)) {
        func_8003BA50(self, old1C);
    }
    self->unk134->unk1E = ((s16)self->unk1E >> 1) - (func_80019668() & 0x3F);
    self->unkFA = 0;
    func_800330E8(self);
    self->unk134->unk18 = 0;
    self->unkF4 = self->unk134->unk24;
    self->unkF8 = self->unk134->unk20;
    D_8009FA94->unkE0 = 4;
}
