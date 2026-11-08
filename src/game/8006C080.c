#include "game/types.h"

extern u8 D_8009FA8C[];
extern volatile s32 D_8009FAA4;
extern s32 D_8009F9EC[];

struct Unk6C080 {
    u8 pad00[0x84];
    s16 field84[3];
    s16 field8A[3];
    u8 field90[3];
    u8 pad93[0x2E];
    s8 fieldC1;
};

struct Arg6C080 {
    u8 pad00[0xAC];
    struct Unk6C080 *ptr;
};

extern void func_8006AAEC(u8 id);
extern void func_800491F8(s32 value, s32 x, s32 y, s32 color);
extern void func_8006BF44(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

void func_8006C080(struct Arg6C080 *arg0)
{
    s32 i;
    struct Unk6C080 *d;
    register s32 flag __asm__("$19");
    s32 x;
    s32 y;
    register u8 *base __asm__("$22");

    i = 0;
    base = D_8009FA8C;
    x = 0x33;
    d = arg0->ptr;
    y = 0x22;
    for (; i < 3; x += 0x29, i++, y += 0x29) {
        func_8006AAEC(d->field90[i]);
        flag = 0;
        if (*(u8 *)((u32)d->fieldC1 + (u32)base) == 0 && d->field8A[i] > 0) {
            flag = 1;
            func_800491F8(d->field8A[i], x, 0xCE, (D_8009FAA4 & 0x40) ? 0x9 : 0xC);
        }
        func_8006BF44(i, (*(u8 *)((u32)d->fieldC1 + (u32)base) != 0 || flag != 0) ? 1 : 0, y, 0xE0);
        func_800491F8((*(u8 *)((u32)d->fieldC1 + (u32)base) != 0) ? d->field8A[i] : d->field84[i], x, 0xD8, 8);
    }
    {
        s32 a0 = -1;
        s32 a2 = 0xC;
        s32 c;
        u8 *arr;
        __asm__("" : "=r"(a0) : "0"(a0));
        __asm__("" : "=r"(a2) : "0"(a2));
        c = d->fieldC1;
        __asm__("" : "=r"(c) : "0"(c));
        __asm__("" : "=r"(c) : "0"(c));
        arr = D_8009FA8C;
        __asm__("" : "=r"(arr) : "0"(arr));
        func_8006BF44(a0, *(u8 *)((u32)c + (u32)arr), a2, 0xE0);
    }
    if (D_8009F9EC[0] != 0) {
        func_8006BF44(-1, 0, 0x124, 0xE0);
    }
}
