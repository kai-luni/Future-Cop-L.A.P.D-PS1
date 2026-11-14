#include "game/types.h"

extern u8 D_800A0968[];
extern volatile u32 D_8009F634;

extern void func_80022D84(void);

void func_80023204(s32 arg0)
{
    u8 *base;
    u8 *p;
    register u32 i __asm__("$4");
    u32 cnt;
    register u32 max __asm__("$3");
    u32 fn;
    u32 a;
    u32 b;
    register u32 prod __asm__("$6");
    u32 sum;
    u32 m;
    u8 *q;

    base = D_800A0968;
    if ((u16)arg0 != *(volatile u16 *)(base + 0x10)) {
        return;
    }
    cnt = *(volatile u32 *)(base + 0x20);
    max = *(volatile u32 *)(base + 0x14);
    cnt = cnt + 1;
    *(volatile u32 *)(base + 0x20) = cnt;
    if (cnt < max) {
        goto do_call;
    }
    i = 0;
    *(volatile u32 *)(base + 0x80) = 1;
    *(u32 *)(base + 0x20) = 0;
    if (max != 0) {
        p = base;
    loop:
        *(volatile u32 *)(p + 0x34) = 0;
        m = *(volatile u32 *)(base + 0x14);
        i++;
        p += 0x24;
        if (i < m) {
            goto loop;
        }
    }
    fn = D_8009F634;
    if (fn != 0) {
        ((void (*)(s32))fn)(3);
    }
    q = D_800A0968;
    __asm__ __volatile__("" ::: "memory");
    if (arg0 < 0) {
        *(volatile u32 *)(q + 0x40) = *(volatile u32 *)(q + 0x78);
    }
    a = *(volatile u32 *)(q + 0x6C);
    b = *(volatile u32 *)(q + 0x70);
    prod = b * a;
    sum = *(volatile u32 *)(q + 0x78) + a;
    *(volatile u32 *)(q + 0x78) = sum;
    if (sum == prod) {
        *(volatile u32 *)(q + 0x78) = 0;
    }
    if (*(volatile u32 *)(q + 0x34) == 0) {
        return;
    }
    if (*(volatile u32 *)(q + 0x74) == 0) {
        return;
    }
do_call:
    func_80022D84();
}
