typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

extern volatile u32 D_8009DEEC;
extern u32 D_8009DEE8;
extern u32 D_8009DEF8;
extern u32 *D_8009DED4;
extern u32 *D_8009DEE4;
extern u32 *D_8009DEC8;
extern u8 D_800A62C8[];
extern u8 D_800A9450[];

extern u32 func_800915FC(u32 a0);
extern u32 func_80095350(u32 a0);
extern void func_80095830(unsigned char *destination, int value, unsigned int count);
extern void func_800951D8(void);
extern void func_80094CEC(void);
extern s32 func_8009520C(void);

s32 func_80094F4C(s32 a0)
{
    u32 saved;

    saved = func_800915FC(0);
    D_8009DEEC = 0;
    D_8009DEE8 = D_8009DEEC;
    D_8009DEF8 = saved;

    switch (a0 & 7) {
    case 0:
    case 5:
        *D_8009DED4 = 0x401;
        *D_8009DEE4 |= 0x800;
        *D_8009DEC8 = 0;
        func_80095830(D_800A62C8, 0, 0x100);
        func_80095830(D_800A9450, 0, 0x1800);
        break;
    case 1:
    case 3:
        *D_8009DED4 = 0x401;
        *D_8009DEE4 |= 0x800;
        *D_8009DEC8 = 0x2000000;
        *D_8009DEC8 = 0x1000000;
        break;
    }

    func_800915FC(D_8009DEF8);
    if ((a0 & 7) == 0) {
        return func_80095350(a0);
    }
    return 0;
}
