#include "game/types.h"

typedef struct {
    void *unk00;
    u8 pad04[0x44];
    s16 unk48;
} Model518A8;

typedef struct {
    u8 pad00[0x34];
    u8 *unk34;
} Table518A8;

typedef struct {
    s16 unk00;
    s16 unk02;
    s16 unk04;
    s16 unk06;
    s16 unk08;
    s16 unk0A;
    s16 unk0C;
    s16 unk0E;
} Elem518A8;

typedef struct {
    u8 pad00[0x40];
    u32 unk40;
    s16 unk44;
    s16 unk46;
} Sub518A8;

typedef struct {
    u8 pad00[0x25C];
    Sub518A8 sub;
    u8 pad2A4[0x2DC - 0x2A4];
    u8 *unk2DC;
} State518A8;

typedef struct {
    u8 pad00[0xAC];
    State518A8 *unkAC;
} Obj518A8;

extern void func_80010750(void *a0);
extern void func_80010234(void *a0, void *a1, s32 a2);
extern s32 func_80095900(s32 a0);

void func_800518A8(Model518A8 *model, s16 *buf, s32 t, Obj518A8 *obj)
{
    Elem518A8 *e;
    Sub518A8 *sub;
    s32 f00;
    s32 f04;
    s32 f06;
    s32 p2;
    s32 p1;
    s32 c1;
    s32 cenx;
    s32 mx, mn;
    s32 rad;
    
    s32 v;

    e = (Elem518A8 *)((u8 *)((Table518A8 *)model->unk00)->unk34
                      + (model->unk48 * 16 + 8));

    p2 = e->unk0A;
    p1 = p2 * t;
    p2 = p2 * ((t >> 4) + 0x1000);
    model = (Model518A8 *)((u8 *)model + 4);
    f04 = e->unk04;
    f06 = e->unk06;
    f00 = e->unk00;

    c1 = 174 + ((-154 * t) >> 16);
    buf[1] = c1;
    buf[5] = c1;
    buf[9] = c1;
    buf[13] = c1;

    f04 += p1 >> 16;
    p2 >>= 12;
    f06 += (f06 * (t >> 2)) >> 16;

    buf[0] = f00 - f06;
    buf[2] = f04 + p2;
    buf[4] = f00 + f06;
    buf[6] = f04 + p2;
    buf[8] = f00 + f06;
    buf[10] = f04 - p2;
    buf[12] = f00 - f06;
    buf[14] = f04 - p2;
    func_80010750(model);
    func_80010234(buf, (s16 *)((u8 *)buf + 0x20), 4);

    cenx = 294 + ((-358 * t) >> 17);
    rad = (51 * (0x10000 - t)) >> 17;
    mx = cenx + rad;
    mn = cenx - rad;

    sub = (Sub518A8 *)((u8 *)obj->unkAC + 0x25C);
    sub->unk44 = cenx;

    cenx = cenx * cenx;

    e = (Elem518A8 *)((u8 *)obj->unkAC->unk2DC + 8);

    e->unk00 = f00;
    e->unk06 = f06;
    f06 = f06 * f06;
    e->unk0A = p2;
    p2 = p2 * p2;
    e->unk02 = mx;
    e->unk04 = f04;
    e->unk08 = mn;

    sub->unk40 = func_80095900(((f06 + cenx) + p2) >> 8) << 2;
    f06 += p2;
    e->unk0C = (s32)sub->unk40 >> 4;
    v = func_80095900((f06) >> 8) << 2;
    sub->unk46 = v >> 4;
    e->unk0E = v >> 4;
}
