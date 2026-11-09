#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk178DC {
    u8 unk00[0x24];
    struct Vec3s32 translation;
};

struct Unk592D0Sub {
    u8 pad00[0xBC];
    s32 unkBC;
    u8 padC0;
    s8 unkC1;
};

struct Unk592D0 {
    u8 pad00[0x50];
    s32 x;
    s32 y;
    s32 z;
    u8 pad5C[0x50];
    struct Unk592D0Sub *unkAC;
};

struct Vertex592D0 {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
};

struct Frame592D0 {
    struct Unk178DC object;
    u8 matrix[0x30];
    struct Vertex592D0 point[7];
    u8 unkA8[0x8];
    s32 near;
    s32 far;
};

extern s32 D_8009FA0C;
extern u8 *D_8009FA94;
extern struct Unk592D0 *D_8009FA58[];
extern u8 D_8009AF40[];
extern u8 *D_8009FA90;
extern u8 *D_8009F9C0;

extern void func_800536B4(s32 *near, s32 *far);
extern s32 func_80095C30(s32 x, s32 y);
extern void func_80018DC0();
extern void func_800178DC(struct Unk178DC *object, s32 arg1);
extern void func_80010750(void *arg0);
extern void func_800101D4(void *arg0, void *arg1, s32 arg2);
extern s32 func_80019E70(s16 *point, s32 margin);
extern void func_8001047C(void *arg0, void *arg1, void *arg2, s32 arg3);

void func_800592D0(struct Unk592D0 *arg0)
{
    struct Frame592D0 frame;
    struct Unk592D0 *other;
    register struct Unk592D0 **table __asm__("$3");
    register s32 index __asm__("$2");
    s32 angle;
    register s32 color __asm__("$6");
    register s32 sum __asm__("$2");
    register s32 point1z __asm__("$3");
    register s32 intensity __asm__("$7");
    s32 temp;
    register u32 word __asm__("$3");
    register s32 height_input __asm__("$4");
    register s32 zero __asm__("$5");
    register s32 anglearg __asm__("$4");
    register s32 height_shift __asm__("$2");
    register s32 height_delta __asm__("$3");
    u8 *p;
    register u32 afbase __asm__("$2");
    register u8 *afptr __asm__("$17");

    if (D_8009FA0C < 2) {
        return;
    }
    if (*(s32 *)(D_8009FA94 + 0x5C) != 0) {
        return;
    }
    func_800536B4(&frame.near, &frame.far);
    if (frame.far == 0) {
        return;
    }

    height_input = arg0->unkAC->unkBC;
    frame.object.translation.x = arg0->x;
    height_shift = (-(height_input * 0x166)) >> 16;
    __asm__("" : "=r"(height_shift) : "0"(height_shift));
    height_delta = height_shift << 4;
    __asm__("" : "=r"(height_delta) : "0"(height_delta));
    height_shift = arg0->y;
    __asm__("" : "=r"(height_shift), "=r"(height_delta) : "0"(height_shift), "1"(height_delta));
    height_delta += 0x27C0;
    height_shift += height_delta;
    __asm__("" : "=r"(height_shift) : "0"(height_shift));
    frame.object.translation.y = height_shift;
    frame.object.translation.z = arg0->z;

    index = 1 - arg0->unkAC->unkC1;
    height_input = arg0->x;
    __asm__ __volatile__("" : "=r"(index), "=r"(height_input) : "0"(index), "1"(height_input) : "$3");
    table = D_8009FA58;
    __asm__("" : "=r"(table) : "0"(table));
    other = table[index];
    angle = func_80095C30(other->x - height_input, other->z - arg0->z);
    anglearg = (angle << 12) >> 12;
    __asm__("" : "=r"(anglearg) : "0"(anglearg));
    zero = 0;
    __asm__("" : "=r"(zero) : "0"(zero));
    func_80018DC0(anglearg, zero, zero, &frame.object);
    func_800178DC(&frame.object, (s32)frame.matrix);
    func_80010750(frame.matrix);

    afbase = 0x800A0000;
    __asm__("" : "=r"(afbase) : "0"(afbase));
    afptr = (u8 *)(afbase - 0x50C0);
    func_800101D4(afptr, frame.point, 1);
    if ((func_80019E70((s16 *)frame.point, 0x400) & 0xFF) != 2) {
        return;
    }
    func_8001047C(afptr, frame.point, frame.unkA8, 7);

    sum = frame.point[0].z;
    point1z = frame.point[1].z;
    __asm__("" : "=r"(sum), "=r"(point1z) : "0"(sum), "1"(point1z));
    sum += point1z;
    __asm__("" : "=r"(sum) : "0"(sum));
    color = sum / 2;
    color -= 0x100;

    if (D_8009FA90 >= D_8009F9C0) {
        p = D_8009FA90;
    } else {
        p = D_8009FA90;
        D_8009FA90 = p + 0x1C;
    }
    p[3] = 1;
    *(u32 *)(p + 4) = 0xE10006A0;
    word = (*(u32 *)p & 0xFF000000) | (color & 0xFFFFFF);
    intensity = 0x7F;
    __asm__("" : "=r"(intensity) : "0"(intensity));
    temp = (frame.far << 7) >> 16;
    if ((u32)temp < 0x80) {
        intensity = temp;
    }
    *(u32 *)p = word;

    temp = intensity << 8;

    if (D_8009FA90 >= D_8009F9C0) {
        p = D_8009FA90;
    } else {
        p = D_8009FA90;
        D_8009FA90 = p + 0x1C;
    }
    *(u32 *)(p + 0x0) = 0x05000000 | color;
    *(u32 *)(p + 0x4) = 0x2A000000 | temp;
    *(u32 *)(p + 0x8) = *(u32 *)&frame.point[0];
    *(u32 *)(p + 0xC) = *(u32 *)&frame.point[1];
    *(u32 *)(p + 0x14) = *(u32 *)&frame.point[2];
    *(u32 *)(p + 0x10) = *(u32 *)&frame.point[3];

    if (D_8009FA90 >= D_8009F9C0) {
        p = D_8009FA90;
    } else {
        p = D_8009FA90;
        D_8009FA90 = p + 0x1C;
    }
    *(u32 *)(p + 0x0) = 0x04000000 | color;
    *(u32 *)(p + 0x4) = 0x22000000 | temp;
    *(u32 *)(p + 0x8) = *(u32 *)&frame.point[4];
    *(u32 *)(p + 0xC) = *(u32 *)&frame.point[0];
    *(u32 *)(p + 0x10) = *(u32 *)&frame.point[5];

    if (D_8009FA90 >= D_8009F9C0) {
        p = D_8009FA90;
    } else {
        p = D_8009FA90;
        D_8009FA90 = p + 0x1C;
    }
    *(u32 *)(p + 0x0) = 0x04000000 | color;
    *(u32 *)(p + 0x4) = 0x22000000 | temp;
    *(u32 *)(p + 0x8) = *(u32 *)&frame.point[5];
    *(u32 *)(p + 0xC) = *(u32 *)&frame.point[0];
    *(u32 *)(p + 0x10) = *(u32 *)&frame.point[1];

    if (D_8009FA90 >= D_8009F9C0) {
        p = D_8009FA90;
    } else {
        p = D_8009FA90;
        D_8009FA90 = p + 0x1C;
    }
    height_shift = 0x04000000 | color;
    __asm__("" : "=r"(height_shift) : "0"(height_shift));
    *(u32 *)(p + 0x0) = height_shift;
    *(u32 *)(p + 0x4) = 0x22000000 | temp;
    *(u32 *)(p + 0x8) = *(u32 *)&frame.point[5];
    *(u32 *)(p + 0xC) = *(u32 *)&frame.point[1];
    *(u32 *)(p + 0x10) = *(u32 *)&frame.point[6];
}
