#include "game/types.h"

typedef struct Node {
    struct Node *next;
    u8 pad04[4];
    s32 unk08;
    volatile s32 unk0C;
    u8 pad10[0x6004];
} Node;

typedef struct {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    Node *unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    u8 unk20;
    u8 pad21[3];
    s32 unk24;
    u8 pad28[4];
    void *unk2C;
    s32 unk30;
    u8 pad34[8];
    u8 unk3C;
    u8 pad3D;
    u16 unk3E;
    u16 unk40;
    u8 pad42[2];
    s32 unk44;
    s32 unk48;
    s32 unk4C;
} State800A0B70;

typedef struct {
    s32 unk00;
    u8 pad04[4];
    s32 unk08;
    u8 pad0C[0xC];
} Unk800A6D78;

struct FixedPool {
    s32 stride;
    u8 *end;
    s32 count;
    s32 available;
    void *free;
};

extern State800A0B70 D_800A0B70;
extern Node *D_8009FA48;
extern s32 D_8009F238;
extern Unk800A6D78 D_800A6D78[];
extern u8 D_800A0C30[0x14];
extern u32 D_800A0C10[];
extern u32 D_800A0C20[];
extern u32 D_800A0C50[];
extern u32 D_800A0C60[];
extern s32 D_800A0B9C[2];
extern u16 D_800A0BC0[];

extern s32 D_8009F698;
extern u8 D_8009F69C;
extern s32 D_8009F6A0;
extern s32 D_8009F6A4;
extern volatile u8 D_8009F6A8;
extern volatile s32 D_8009F6AC;
extern volatile s32 D_8009F6B0;
extern volatile s32 D_8009F6B4;
extern u8 D_8009F6B8;
extern s32 D_8009F6BC;
extern u8 D_8009F6C0;
extern s32 D_8009F6C4;
extern s32 D_8009F6CC;
extern s32 D_8009F6D0;
extern s32 D_8009F6D4;
extern s32 D_8009F6D8;
extern s32 D_8009F6DC;
extern s32 D_8009F6E4;
extern s32 D_8009F6E8;
extern s32 D_8009F9B0;

extern void *func_80017EBC(s32 size, s32 tag);
extern struct FixedPool *func_800194DC(s32 count, s32 size, s32 tag);
extern s32 func_8001D5E4(void *arg0, s32 arg1);

void func_80029F50(void *arg0)
{
    State800A0B70 *state;
    Node *node;
    Node *prev;
    u32 index;
    s32 value;
    s32 i;
    s32 count;
    s32 stride;

    D_800A0C30[0x10] = 0;
    D_800A0C30[0x11] = 0;
    D_800A0C30[0x12] = 0;
    D_800A0C30[0x13] = 0xFF;

    D_8009F6A8 = 0;
    D_8009F6AC = 0;
    D_8009F6B0 = 0;
    D_8009F6B4 = 0;
    D_8009F9B0 = 0;
    D_8009F6B8 = 0;
    D_8009F6BC = -1;
    D_8009F6C0 = 0;
    D_8009F6C4 = 0;
    D_8009F69C = 0;
    D_8009F698 = 0;
    D_8009F6A0 = 0;
    D_8009F6A4 = D_8009F6B4;

    for (i = 0; i < 4; i++) {
        D_800A0C10[i] = 0;
        D_800A0C20[i] = 0;
    }

    stride = 0x6014;
    D_800A0B9C[0] = 0;
    node = func_80017EBC(0x18050, 2);
    D_8009FA48 = node;

    prev = 0;
    count = 4;
    do {
        node->next = prev;
        node->unk08 = 0;
        node->unk0C = 0;
        prev = node;
        node = (Node *)((u8 *)prev + stride);
    } while (--count != 0);

    state = &D_800A0B70;
    state->unk1C = 0x6000;
    state->unk3C = 1;
    state->unk40 = 0xFFFF;
    state->unk10 = prev;
    state->unk14 = 0;
    state->unk18 = 0;
    state->unk08 = 0;
    state->unk24 = 0;
    state->unk20 = 0;
    state->unk30 = 0;
    state->unk3E = 0;
    state->unk44 = 0;
    state->unk48 = 0;
    state->unk4C = 0;
    state->unk0C = -2;
    state->unk2C = func_800194DC(0x280, 8, 1);

    index = func_8001D5E4(arg0, D_8009F238);
    state->unk00 = index;
    value = -9;
    if (index < 4) {
        if (D_800A6D78[index].unk00 != 0) {
            value = D_800A6D78[index].unk08;
        }
    }
    state->unk04 = value;

    D_800A0BC0[0] = 0;
    D_800A0BC0[1] = 0;

    for (i = 0; i < 4; i++) {
        D_800A0C50[i] = 0;
        D_800A0C60[i] = 0;
    }

    D_8009F6CC = 0;
    D_8009F6D0 = 0;
    D_8009F6D4 = 0;
    D_8009F6D8 = 0;
    D_8009F6DC = 0;
    D_8009F6E4 = 0;
    D_8009F6E8 = 1;
}
