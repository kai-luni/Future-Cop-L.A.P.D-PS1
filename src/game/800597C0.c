#include "game/types.h"

struct Unk17508 {
    void *model;
};

struct State597C0;
struct Info597C0;

struct Object597C0 {
    u8 unk00[0x24];
    u32 flags;
    struct Unk17508 instance;
    u8 unk2C[0x88];
    struct State597C0 *state;
};

struct State597C0 {
    u8 unk00[0x24];
    u32 flags;
    struct Unk17508 instance;
    u8 unk2C[0x66];
    u8 unk92;
    u8 unk93;
    u8 unk94[0x18];
    struct Info597C0 *info;
};

struct Model597C0Sub {
    u8 unk00[0xC];
    u8 unk0C;
    u8 unk0D;
};

struct Model597C0 {
    u8 unk00[8];
    struct Model597C0Sub *unk08;
};

struct Info597C0 {
    u8 unk00[0xBC];
    s32 unkBC;
    u8 unkC0[0xB8];
    struct Unk17508 unk178;
    u8 unk17C[0x48];
    struct Unk17508 unk1C4;
    u8 unk1C8[0x48];
    struct Unk17508 unk210;
    u8 unk214[0x104];
    s16 unk318;
    u8 unk31A[0xA];
    s16 unk324;
};

struct Camera597C0 {
    u8 unk00[0x28];
    u8 unk28[0x54];
    s32 unk7C;
    s32 unk80;
    s32 unk84;
};

struct Global597C0 {
    u8 unk00[0x5C];
    s32 unk5C;
};

extern u8 D_8009FAA4[];
extern u8 D_8009AF3C[];
u8 D_8009F36D;
extern u8 D_8009F36E;
extern u8 D_8009F36F;
extern s32 D_8009F370;
extern struct Camera597C0 *D_8009F9E4;
extern struct Global597C0 *D_8009FA94[];

extern void func_80013AA0(s32 low, s32 high);
extern u8 func_80017508(struct Unk17508 *instance, u32 *flags, s32 arg2);
extern void func_80017668(struct Unk17508 *instance, s32 part, s32 *out);
extern s32 func_80019D90(s16 *point, s32 margin);
extern s32 func_800584BC(s32 message, struct State597C0 *object, void *hit, s32 arg3, s32 arg4);
extern void func_8005920C(struct Object597C0 *object, void *spawn, s32 arg2);
extern void func_800592D0(struct State597C0 *state);
extern void func_800596C4(struct State597C0 *state);
extern void func_8001010C(s16 *a, s16 *b, s32 c);
extern void func_8001047C(s16 *a, s16 *b, s16 *c, s32 d);
extern void func_80010820(u8 *a);
extern void func_8001484C(s16 *a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g);
extern s32 func_8002AD58(s32 kind, struct Object597C0 *object, void *arg2, s32 arg3);

void func_800597C0(struct Object597C0 *object)
{
    struct State597C0 *state;
    struct Info597C0 *info;
    register struct Camera597C0 *cam __asm__("$6");
    s32 arg2;
    register s32 i __asm__("$18");
    s32 offset;
    s32 count;
    u8 *color;
    u8 *parts;
    s32 dx;
    s32 dy;
    s32 dz;
    struct {
        s32 from[3];
        u8 pad0[4];
        s16 point[4];
        s16 coords[4];
        s16 hit[4];
    } loc;

    state = object->state;
    if (state->info->unk318 > 0) {
        return;
    }
    info = state->info;

    {
        register s32 low __asm__("$4");
        register s32 high __asm__("$5");
        s32 condition = info->unk324;
        __asm__ volatile("" : "+r"(condition));
        low = state->unk92;
        __asm__ volatile("" : "+r"(low));
        if (condition > 0) {
            register s32 value __asm__("$2");
            __asm__ volatile("" : : : "memory");
            value = *(volatile u8 *)D_8009FAA4;
            __asm__ volatile("" : "+r"(value));
            high = value >> 2;
        } else {
            high = state->unk93;
        }
        func_80013AA0(low, high);
    }

    arg2 = 1;
    if (info->unkBC != 0) {
        arg2 = 5;
    }
    if (info->unkBC != 0x10000) {
        arg2 |= 2;
    }
    func_80017508(&state->instance, &state->flags, arg2);

    arg2 = 1;
    if (info->unkBC != 0) {
        arg2 = 0xB;
    }
    if (info->unkBC != 0x10000) {
        arg2 |= 4;
    }
    func_80017508(&object->instance, &object->flags, arg2);

    func_80017508(&info->unk178, &object->flags, 1);
    func_80017508(&info->unk1C4, &object->flags, 1);
    func_80013AA0(0, 0);
    func_80017508(&info->unk210, &object->flags, 1);
    func_800592D0(state);

    offset = 0;
    i = offset;
    __asm__ volatile("" : "+r"(i));
    color = &D_8009F36D;
    parts = D_8009AF3C;
    count = ((struct Model597C0 *)info->unk210.model)->unk08->unk0C;

    do {
        struct Model597C0 *model = (struct Model597C0 *)info->unk210.model;
        s32 temp = model->unk08->unk0D + offset;
        s32 rem = temp % count;
        s32 quotient = (rem * 255) / count;

        D_8009F36E = 0;
        D_8009F36F = 0;
        D_8009F36D = quotient;

        func_80017668(&info->unk210, *(u8 *)(i + (u32)parts), loc.from);

        cam = D_8009F9E4;
        loc.from[0] -= cam->unk7C;
        loc.from[1] -= cam->unk80;
        loc.from[2] -= cam->unk84;
        loc.coords[0] = loc.from[0] >> 4;
        loc.coords[1] = loc.from[1] >> 4;
        loc.coords[2] = loc.from[2] >> 4;

        func_80010820((u8 *)cam + 0x28);
        func_8001010C(loc.coords, loc.point, 1);

        if ((u8)func_80019D90(loc.point, 0x700) == 2) {
            func_8001047C(loc.coords, loc.point, loc.hit, 1);
            func_8001484C(loc.point, *(s32 *)(color - 1), D_8009F370, 4, 0x70, 0x200, 0);
            offset += count >> 1;
        }
        i++;
    } while (i < 2);

    func_800596C4(object->state);
}
