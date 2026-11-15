#include "game/types.h"

struct PacketState {
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    s16 values[4];
    u8 flags[4];
};

struct Text8 {
    char b[8];
};

extern struct Text8 D_8009F3B8;

extern void func_80091480(void);
extern void func_80091510(void *);
extern void func_80092910(void *);
extern void func_80092E10(void *, int, int, int);
extern void *func_80092140(void *, int, int, int, int);
extern void *func_800921F4(void *, int, int, int, int);
extern void func_800934BC(void *);
extern void func_800932F0(void *);
extern void func_80092BF4(int);
extern void func_80077278(int, int, unsigned int);

void func_8007757C(int a0, int a1, int a2, int a3)
{
    u8 disp[0xB8];
    struct PacketState st[2];
    struct Text8 text;

    text = D_8009F3B8;

    func_80091480();
    func_80091510(0);
    func_80092910(0);
    func_80092E10(&text, 0, 0, 0);
    func_80092140(disp, 0, 0, 0x140, 0xF0);
    func_800921F4(st, 0, 0, 0x140, 0xF0);
    func_800934BC(st);
    func_800932F0(disp);
    func_80092BF4(1);

    func_80077278(0x20, 0x20, a0);
    func_80077278(0x20, 0x40, a1);
    func_80077278(0x20, 0x60, a2);
    func_80077278(0x20, 0x80, a3);

    __asm__ volatile(".word 0x000101CD");
}
