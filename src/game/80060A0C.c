#include "game/types.h"

struct Unk3BObject {
    u8 unk00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 tableIndex;
    u8 unk21[3];
    u32 flags;
};

struct Unk5A190Gauge {
    s32 unk00;
    s32 value;
    s32 unk08;
    s32 limit;
    u8 state;
    u8 unk11[3];
};

struct UnkFA94 {
    u8 pad00[0x40];
    s32 unk40;
    s32 unk44;
    u8 pad48[0x14];
    s32 unk5C;
    u8 pad60[4];
    s32 unk64[0x100];
};

struct ActorSub {
    u8 pad00[0x54];
    struct Unk5A190Gauge gauge;
    u8 pad68[0x59];
    s8 unkC1;
    u8 padC2[0x1D];
    u8 unkDF;
};

struct Actor {
    u8 pad00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 tableIndex;
    u8 pad21[3];
    u32 flags;
    u8 pad28[0x6A];
    u8 unk92;
    u8 pad93[0x19];
    struct ActorSub *unkAC;
};

extern struct Actor *D_8009FA58[];
extern struct Actor *actor0 __asm__("D_8009FA58");
extern struct UnkFA94 *D_8009FA94;
extern s32 D_8009FA0C;
extern s32 D_8009FA00;
extern s32 D_8009FA70;
extern u8 D_8009FA8C[];

extern u32 func_80060DD8(u32 state);
extern void func_8005A0C4(struct Unk5A190Gauge *gauge, s32 value, s32 divisor);
extern void func_8003BA50(struct Unk3BObject *object, s32 value);
extern void func_8006BDFC(struct Actor *actor, s32 mode);
extern void func_8006BE90(struct Actor *actor, s32 mode, s32 arg2, s32 arg3);

s32 func_80060A0C(u32 state, s32 index)
{
    s32 result;
    s32 old;
    register u32 offset __asm__("$2");
    register s32 delta __asm__("$3");
    register struct Actor *actor __asm__("$4");

    state = func_80060DD8(state);
    result = 1;
    switch (state) {
    case 0x00011081:
    label_11081:
        old = D_8009FA58[index]->unk1C;
        D_8009FA58[index]->unk1C = D_8009FA58[index]->unk1E;
        func_8005A0C4(&D_8009FA58[index]->unkAC->gauge,
                      D_8009FA58[index]->unk1C,
                      D_8009FA58[index]->unk1E);
        if (D_8009FA58[index]->tableIndex != 0 &&
            (D_8009FA58[index]->flags & 0x40000) == 0) {
            func_8003BA50((struct Unk3BObject *)D_8009FA58[index], old);
        }
        break;

    case 0x01ADCFED:
        offset = 1;
        if (D_8009FA94->unk5C == offset) {
            D_8009FA70 = 0x78;
            delta = index - 1;
            D_8009FA94->unk40 = D_8009FA94->unk44 - delta;
        } else {
            D_8009FA00 = 0x0B;
            D_8009FA94->unk40 = D_8009FA94->unk44;
        }
        break;

    case 0x016499A4:
        offset = 1;
        if (D_8009FA0C == offset) {
            actor = actor0;
            if (actor != 0) {
                actor->unk92 = 0x80 - actor->unk92;
            }
        }
        break;

    case 0x031F78DF:
        __asm__ volatile ("" ::: "memory");
        D_8009FA8C[index] = 1;
        break;

    case 0x01BD6EFD:
        func_8006BE90(D_8009FA58[index], 0, -1, 0);
        break;

    case 0x03449B84:
        offset = index * 4;
        func_8006BE90(*(struct Actor **)((u32)D_8009FA58 + offset), 1, -1, 0);
        break;

    case 0x0058E758:
        func_8006BE90(D_8009FA58[index], 2, -1, 0);
        break;

    case 0x032D496D:
        func_8006BDFC(D_8009FA58[index], 0);
        break;

    case 0x004F464F:
        offset = index * 4;
        func_8006BDFC(*(struct Actor **)((u32)D_8009FA58 + offset), 1);
        break;

    case 0x0063A563:
        offset = index * 4;
        func_8006BDFC(*(struct Actor **)((u32)D_8009FA58 + offset), 2);
        break;

    case 0xFEDAE5DA:
        offset = index * 4;
        (*(struct Actor **)((u32)D_8009FA58 + offset))->unkAC->unkDF |= 1;
        break;

    case 0x00F8F778:
        D_8009FA94->unk64[D_8009FA58[index]->unkAC->unkC1] += 0xC8;
        break;

    case 0x0A9CF51C:
        D_8009FA58[index]->unk1E = 0x7FFF;
        goto label_11081;

    default:
        result = 0;
        break;
    }
    return result ^ 1;
}
