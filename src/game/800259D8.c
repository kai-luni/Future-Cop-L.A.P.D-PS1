#include "game/types.h"

typedef struct {
    u8 unk00[0x10];
    s32 unk10;
} UnkFA94;

extern UnkFA94 *D_8009FA94;

extern s32 D_8009F9C4;
extern s32 D_8009F9FC;
extern s32 D_8009FA00;
extern s32 D_8009FA30;
extern volatile u8 D_8009FA3C;
extern s32 D_8009FAA4;

extern u32 *D_8009F9F0;
extern u32 *D_8009FA28;
extern volatile s32 D_8009F9D4;
extern u8 *D_8009F9A0;
extern u8 *D_8009FA58[];

extern void func_8001BFD8(u8 value);
extern void func_80025370(void);
extern void func_80086134(void);
extern void func_80092660(u32 *arg0, s16 *arg1, s32 arg2, s32 arg3);
extern void func_8001B4A8(s32 arg0);
extern u8 func_8002385C(void);
extern void func_80025690(void);
extern void func_80024EC8(void);
extern void func_80047690(u8 enable);
extern void func_8006AB38(void *arg0);
extern void func_80056634(void);

s32 func_800259D8(void)
{
    u32 *p;
    volatile s16 rect[4];
    s32 i;
    s32 one;
    register s32 color __asm__("$7");
    register s32 mode __asm__("$16");

    D_8009FA30 = 0;
    D_8009FAA4 = 0;
    D_8009F9FC = 5;

    func_8001BFD8(0);
    func_80025370();

    if (D_8009F9C4 == 0) {
        func_80086134();
    }

    while (D_8009FA3C != 0) {
    }

    p = D_8009F9F0;
    if (p < D_8009FA28) {
        D_8009F9F0 = p + 0xD;
    }

    {
        register u32 masked __asm__("$2");
        register u32 half __asm__("$3");
        register u32 bits __asm__("$3");
        register volatile s16 *rp __asm__("$5");
        register s32 zero __asm__("$6");
        register u32 height __asm__("$2");
        mode = 4;
        __asm__("" : "+r"(mode));
        bits = 0xFF000000;
        rp = rect;
        __asm__("" : "+r"(rp));
        masked = p[0];
        color = D_8009F9D4;
        masked = masked & bits;
        __asm__("" : "+r"(masked) : : "$6");
        half = *(volatile u16 *)&D_8009F9D4;
        __asm__("" : "+r"(half) : : "$6");
        zero = 0;
        __asm__("" : "+r"(zero) : "r"(half));
        p[0] = masked;
        height = 0x100;
        __asm__("" : "+r"(height));
        rect[0] = 0;
        __asm__("" : "+r"(half) : : "memory");
        rect[1] = height - half;
        rect[2] = 0x140;
        *(s16 *)&rect[3] = 0xF0;
        func_80092660(p, rp, zero, color);
    }
    func_8001B4A8(1);

    while (1) {
        if (D_8009FA00 == mode && func_8002385C() != 0) {
            break;
        }
        func_80025690();
        func_80024EC8();
    }

    func_8001BFD8(1);
    func_80047690(1);

    {
        register u8 **cursor __asm__("$16");

        i = 0;
        cursor = D_8009FA58;
        do {
            if (*cursor != 0) {
                func_8006AB38(*cursor);
            }
            i++;
            cursor++;
        } while (i < 2);
    }

    one = 1;
    if (D_8009F9A0 != 0) {
        func_80056634();
        one = 1;
    }

    D_8009FAA4 = 0;
    D_8009FA30 = 0;
    D_8009FA94->unk10 = one;
    return one;
}
