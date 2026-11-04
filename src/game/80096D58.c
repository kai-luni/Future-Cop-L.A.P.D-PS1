#include "game/types.h"

struct Unk96D58 {
    s32 unk0;
    u16 unk4;
    u16 unk6;
    s32 unk8;
    s32 unkC;
};

extern struct Unk96D58 D_8009EA48;
extern s32 D_8009EA38;
extern s32 D_8009EA3C;
extern s32 D_8009EA40;
extern s32 D_8009EA34;
extern s32 D_8009EA30;
extern s32 D_8009EA5C;
extern s32 D_8009EA58;
extern s32 D_8009EEC0;
extern s32 D_8009EE90;
extern s32 D_8009EF04;
extern s32 D_8009EF08;
extern s32 D_8009EF0C;
extern u32 D_8009EF14;
extern u16 D_8009EA8E[];

extern void func_80091480(void);
extern void func_80096EB8(s32 arg);
extern void func_80096E40(void);
extern void func_800977C4(s32 index, u32 value, s32 flag);

void func_80096D58(s32 cold_start)
{
    u16 fill;
    u16 *p;
    s32 i;
    u32 val;
    s32 idx;
    struct Unk96D58 *q;

    func_80091480();
    func_80096EB8(cold_start);

    fill = 0xC000;

    if (cold_start == 0) {
        i = 0x17;
        p = D_8009EA8E;
        for (; i >= 0; i--) {
            *p = fill;
            p--;
        }
    }

    func_80096E40();

    idx = 0xD1;
    __asm__ volatile("" : "=r"(idx) : "0"(idx));
    q = &D_8009EA48;
    __asm__ volatile("" : "=r"(q) : "0"(q));
    val = D_8009EF14;
    D_8009EA38 = 0;
    D_8009EA3C = 0;
    q->unk0 = 0;
    q->unk4 = 0;
    q->unk6 = 0;
    q->unk8 = 0;
    q->unkC = 0;
    D_8009EA40 = val;
    func_800977C4(idx, val, 0);

    D_8009EF04 = 0;
    D_8009EF08 = 0;
    D_8009EF0C = 0;
    D_8009EA34 = 0;
    D_8009EEC0 = 0;
    D_8009EA30 = 0;
    D_8009EA5C = 0;
    D_8009EA58 = 0;
    D_8009EE90 = 0;
    __asm__ volatile("" ::: "memory");
}
