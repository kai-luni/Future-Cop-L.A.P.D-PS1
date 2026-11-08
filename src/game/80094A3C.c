#include "game/types.h"

struct QueueEntry {
    void (*func)();
    void *arg1;
    s32 arg2;
    u32 data[21];
};

struct StructDDF0 {
    u8 unk0;
    u8 unk1;
    u8 pad[6];
    s32 unk8;
    s32 unkC;
};

extern volatile struct QueueEntry D_800A9450[];
extern struct StructDDF0 D_8009DDF0;
extern volatile s32 D_8009DEE8;
extern volatile s32 D_8009DEEC;
extern u32 D_8009DEF0;
extern volatile u32 *D_8009DED4;
extern volatile u32 *D_8009DEC8;

extern void func_800951D8(void);
extern s32 func_8009520C(void);
extern void func_80094CEC(void);
extern u32 func_800915FC(u32);
extern void func_800914E0(s32, void (*)(void));

s32 func_80094A3C(void (*func)(), void *arg1, s32 arg2, s32 arg3)
{
    s32 i;
    u32 *src;
    register volatile u32 (*payload)[24] __asm__("$8");
    register s32 offset __asm__("$4");
    register void *copy __asm__("$2");
    register void **dest __asm__("$1");
    register struct StructDDF0 *ddf0 __asm__("$4");

    func_800951D8();

    while (((D_8009DEE8 + 1) & 0x3F) == D_8009DEEC) {
        if (func_8009520C() != 0) {
            return -1;
        }
        func_80094CEC();
    }

    D_8009DEF0 = func_800915FC(0);
    ddf0 = &D_8009DDF0;
    __asm__("" : "=r"(ddf0) : "0"(ddf0));
    ddf0->unk8 = 1;

    if (ddf0->unk1 == 0 || (D_8009DEE8 == D_8009DEEC && (*D_8009DED4 & 0x1000000) == 0 && ddf0->unkC == 0)) {
        while ((*D_8009DEC8 & 0x04000000) == 0) {
        }
        func(arg1, arg3);
        func_800915FC(D_8009DEF0);
        return 0;
    }

    func_800914E0(2, func_80094CEC);

    i = 0;
    if (arg2 != 0) {
        payload = (volatile u32 (*)[24])D_800A9450[0].data;
        __asm__("" : "=r"(payload) : "0"(payload));
        for (src = (u32 *)arg1; i < arg2 / 4; i++) {
            payload[D_8009DEE8][i] = *src++;
        }
        offset = D_8009DEE8 * 96;
        copy = (void *)D_800A9450[D_8009DEE8].data;
        __asm__ volatile("lui %0,%%hi(D_800A9450+4)\n\taddu %0,%0,%1" : "=r"(dest) : "r"(offset));
        *(void **)((u8 *)dest - 0x6BAC) = copy;
    } else {
        D_800A9450[D_8009DEE8].arg1 = arg1;
    }

    D_800A9450[D_8009DEE8].arg2 = arg3;
    D_800A9450[D_8009DEE8].func = func;
    __asm__ volatile("" ::: "memory");

    D_8009DEE8 = (D_8009DEE8 + 1) & 0x3F;
    func_800915FC(D_8009DEF0);
    func_80094CEC();

    i = (D_8009DEE8 - D_8009DEEC) & 0x3F;
    __asm__ volatile(".set\tnoreorder\n\t.set reorder");
    return i;
}
