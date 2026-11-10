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
extern u32 D_800A6E58[];
extern u32 D_800A6E38[];

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

__attribute__((noreturn)) void func_800773D8(void)
{
    u8 disp[0xB8];
    struct PacketState st[2];
    struct Text8 text;
    int i;
    int j;

    {
        register unsigned int hi __asm__("$2") = 0x800A0000;
        __asm__ volatile(
            "addiu $6,%0,%%lo(D_8009F3B8)\n\t"
            "lwl $3,3($6)\n\tlwr $3,0($6)\n\t"
            "lwl $4,7($6)\n\tlwr $4,4($6)\n\t"
            "swl $3,251($sp)\n\tswr $3,248($sp)\n\t"
            "swl $4,255($sp)\n\tswr $4,252($sp)"
            : : "r"(hi) : "$3", "$4", "$6", "memory");
    }

    func_80091480();
    func_80091510(0);
    func_80092910(0);

    func_80092E10(&text, 0, 0, 0);
    func_80092140(disp, 0, 0, 0x140, 0xF0);
    func_800921F4(st, 0, 0, 0x140, 0xF0);
    func_800934BC(st);
    func_800932F0(disp);
    func_80092BF4(1);

    for (j = 0; j < 4; j++) {
        for (i = 0; i < 8; i++) {
            func_80077278(j * 0x40 + 0x10, i * 0x10 + 0x10, D_800A6E58[j * 8 + i]);
        }
    }

    for (j = 0; j < 2; j++) {
        for (i = 0; i < 4; i++) {
            func_80077278(i * 0x40 + 0x10, j * 0x10 + 0xA0, D_800A6E38[j * 4 + i]);
        }
    }

    for (;;) {
    }
}
