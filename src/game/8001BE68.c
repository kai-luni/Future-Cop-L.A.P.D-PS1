typedef unsigned char u8;
typedef short s16;
typedef int s32;
struct Display { u8 pad0[8]; s16 x, y; u8 pad1[10]; u8 flag; u8 rest[0x45]; };
struct Draw { u8 data[0x14]; };
extern struct Display D_800A06E0[];
extern struct Draw D_800A06B0[];
extern u8 D_8009F225;
extern u8 D_8009F608;
extern void func_80092910(int);
extern void func_80092BF4(int);
extern void func_80092A84(int);
extern void func_80092140(struct Display *, int, int, int, int);
extern void func_800921F4(struct Draw *, int, int, int, int);
int func_8001BE68(void)
{
    int i;
    register int one __asm__("$3");
    register struct Display *p __asm__("$4");
    func_80092910(0);
    func_80092BF4(0);
    func_80092A84(0);
    func_80092140(&D_800A06E0[0], 0, 0, 0x140, 0xF0);
    func_80092140(&D_800A06E0[1], 0, 0x100, 0x140, 0xF0);
    func_800921F4(&D_800A06B0[0], 0, 0, 0x140, 0xF0);
    func_800921F4(&D_800A06B0[1], 0, 0x100, 0x140, 0xF0);
    D_800A06E0[0].x = 0;
    D_800A06E0[0].y = 0;
    D_800A06E0[1].x = 0;
    D_800A06E0[1].y = 0x100;
    i = 0;
    if (i < D_8009F225) {
        one = 1;
        __asm__ volatile("" : "=r"(one) : "0"(one));
        p = D_800A06E0;
        do {
            p->flag = one;
            p++;
            i++;
        } while (i < D_8009F225);
    }
    func_80092BF4(1);
    D_8009F608 = 1;
    return 0;
}
