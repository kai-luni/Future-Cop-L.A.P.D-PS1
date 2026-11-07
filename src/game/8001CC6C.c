#include "game/types.h"

typedef struct {
    volatile s32 unk00;
    s32 unk04;
    volatile s32 unk08;
    s32 unk0C;
    s32 unk10;
    void (*unk14)(s32, s32);
    s32 unk18;
    s32 unk1C;
    u8 *unk20;
    u8 unk24;
} GlobalState;

typedef struct {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
} StreamEntry;

extern GlobalState func_80010000;
extern StreamEntry D_800A6D78[];
extern u8 D_800A85C0[];

extern void func_8008CD10(void *dest, const void *src, s32 n);

void func_8001CC6C(void)
{
    s32 chunk;
    s32 offset;

    func_80010000.unk0C = 0;
    offset = D_800A6D78[func_80010000.unk10].unk0C & 0x7FF;

    chunk = func_80010000.unk04 + offset;
    if (chunk > 0x800) {
        chunk = 0x800;
    }
    chunk -= offset;

    func_8008CD10(func_80010000.unk20, &D_800A85C0[offset], chunk);

    func_80010000.unk20 += chunk;
    func_80010000.unk04 -= chunk;
    __asm__ volatile("");
    func_80010000.unk08 += chunk;
    D_800A6D78[func_80010000.unk10].unk0C += chunk;

    if (func_80010000.unk04 <= 0) {
        func_80010000.unk00 = 0;
        D_800A6D78[func_80010000.unk10].unk00 = 1;
        if (func_80010000.unk24 == 1) {
            D_800A6D78[func_80010000.unk10].unk00 |= 0x100;
        }
        func_80010000.unk14(func_80010000.unk08, func_80010000.unk0C);
    }
}
