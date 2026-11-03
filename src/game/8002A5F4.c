#include "game/types.h"

struct Vec3 { s32 x, y, z; };
struct Slot { u8 active, pad; };
struct Source {
    u16 unk00;
    u8 pad02[2];
    u8 unk04;
    u8 pad05[3];
    struct Vec3 position;
    u32 flags;
    u16 unk18, unk1A;
    u8 unk1C, unk1D, unk1E, unk1F;
    u8 unk20, unk21, unk22, unk23;
};
struct Offset { u8 pad00[0x24]; s32 x, y, z; };
struct Asset {
    u8 pad00[8];
    void **model;
    u8 pad0C[4];
    void **other;
};
struct Args {
    u8 pad00[4];
    struct Asset *asset;
    struct Source *source;
    struct Offset *offset;
};
struct Object {
    u8 pad00[0xC];
    struct Slot slots[4];
    u16 unk14;
    u8 unk16, unk17, unk18, unk19;
    u16 unk1A, unk1C, unk1E;
    u8 unk20, unk21;
    s8 unk22;
    u8 unk23;
    u32 flags;
    void *model;
    u8 pad2C[0x24];
    struct Vec3 position, previous;
    s32 unk68;
    u16 unk6C, unk6E;
    u8 pad70[2];
    u8 unk72, pad73;
    void *other;
    s32 unk78, unk7C, unk80, unk84, unk88;
    u8 pad8C[4];
    u16 unk90;
    u8 unk92, unk93;
    s32 unk94, unk98, unk9C;
};
extern void func_80013BD8(void *, void *, s32);
extern void func_8003FA34(struct Object *);

void func_8002A5F4(struct Object *self, struct Args *args)
{
    struct Source *src;
    struct Offset *off;
    struct Asset *asset;
    register void **link __asm__("$3");
    s32 i;
    register s32 diff __asm__("$3");
    register s32 clamped __asm__("$2");
    void *model;

    for (i = 3; i >= 0; i--) {
        self->slots[i].active = 0;
    }
    off = args->offset;
    src = args->source;
    if (off != 0) {
        self->unk94 = off->x;
        self->unk98 = off->y;
        self->unk9C = off->z;
    } else {
        self->unk98 = 0;
        self->unk94 = 0;
        self->unk9C = 0;
    }
    self->unk92 = src->unk22;
    self->unk93 = src->unk23;
    self->unk18 = src->unk1C;
    self->unk14 = src->unk00;
    self->unk17 = src->unk1D;
    self->unk16 = src->unk04;
    diff = src->unk1F - 15;
    clamped = diff >> 31;
    clamped &= diff;
    clamped += 15;
    self->unk19 = clamped;
    self->unk20 = src->unk20;
    self->unk21 = src->unk21;
    self->unk22 = -1;
    self->unk1C = src->unk18;
    self->unk1A = src->unk1A;
    self->unk23 = src->unk1E;
    self->flags = src->flags;
    self->unk1E = self->unk1C;
    self->flags &= 0xFFBFFFFF;
    if (self->flags & 0x80) {
        self->flags |= 0x01000000;
    }
    model = 0;
    asset = args->asset;
    if (asset != 0) {
        link = asset->model;
        func_80013BD8(&self->model, link != 0 ? *link : 0, self->flags);
        link = asset->other;
        if (link != 0) {
            self->other = *link;
        } else {
            __asm__("" : "+r"(link));
            self->other = 0;
        }
    } else {
        self->other = 0;
        self->model = 0;
        self->unk68 = 0;
        self->unk6C = 0;
        self->unk6E = 0;
        self->unk72 = 0;
    }
    self->position = src->position;
    self->previous = src->position;
    self->unk78 = 0;
    self->unk7C = 0;
    self->unk80 = 0;
    self->unk84 = 0;
    self->unk88 = 0;
    self->unk90 = 0x7F00;
    func_8003FA34(self);
}
