#include "game/types.h"

typedef struct {
    s16 unk00;
    s8 unk02;
    u8 pad03;
    void *unk04;
} Entry29C34;

typedef struct {
    s16 count;
    s16 index;
    Entry29C34 entries[8];
} Queue29C34;

typedef struct {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    u8 pad10[0x10];
    u8 unk20;
    u8 pad21[0x1B];
    u8 unk3C;
    u8 pad3D[0xF];
    s32 unk4C;
} State29C34;

extern Queue29C34 D_800A0BC0;
extern State29C34 D_800A0B70;
extern u8 D_8009F6B8;
extern s32 D_8009F6BC;
extern u8 D_8009F6C0;
extern s32 D_8009F6C4;
extern s32 D_8009F6DC;
extern u8 D_8009F69C;
extern u8 *D_8009F698;
extern s32 D_8009FA2C;
extern u8 D_800A0B90[];

extern void func_80044FF8(s32 mode, void *object);
extern s32 func_80029BE4(void);
extern void func_8002A49C(void);
extern void func_8002A408(void);
extern void func_80029520(void);

void func_80029C34(void)
{
    State29C34 *state;
    s32 index;
    s32 value;

    state = &D_800A0B70;

    if (D_800A0BC0.count <= 0) {
        return;
    }
    if (D_8009F6B8 & 0x70) {
        return;
    }

    index = D_800A0BC0.index;
    value = D_800A0BC0.entries[index].unk00;

    if (D_800A0BC0.entries[index].unk02 != 0) {
        func_80044FF8(value, D_800A0BC0.entries[index].unk04);
    } else {
        func_80044FF8(-1, 0);
    }

    if (value == D_8009F6BC) {
        D_8009F6B8 &= 0xF7;
        goto tail;
    }

    if (D_8009F6DC == 3) {
        return;
    }

    if ((func_80029BE4() & 0xFF) != 1) {
        return;
    }

    if (D_8009F6DC != 2 && D_8009F6DC != 0) {
        D_800A0B90[0] = 1;
        func_8002A49C();
        func_8002A408();
        D_8009F6DC = 2;
    }

    D_8009F6BC = value;
    D_8009F6B8 = 0x40;
    D_8009F6C0 = 0;
    D_8009F6C4 = D_8009FA2C + 0xBB8;
    func_80029520();

tail:
    D_8009F69C = 0;
    D_800A0BC0.index = (D_800A0BC0.index + 1) & 7;
    D_800A0BC0.count = D_800A0BC0.count - 1;

    if (D_8009F6B8 == 0x40) {
        state->unk3C = 0;
        state->unk20 = 0;
        state->unk0C = value;
        state->unk08 = state->unk4C + *(s32 *)(D_8009F698 + value * 12 + 4);
    }
}
