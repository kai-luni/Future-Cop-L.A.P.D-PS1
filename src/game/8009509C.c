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

u32 func_8009509C(u32 a0)
{
    u32 count;

    if (a0 == 0) {
        func_800951D8();
        while (D_8009DEE8 != D_8009DEEC) {
            func_80094CEC();
            if (func_8009520C() != 0) {
                return -1;
            }
        }
        while ((*D_8009DED4 & 0x1000000) || !(*(volatile u32 *)D_8009DEC8 & 0x4000000)) {
            if (func_8009520C() != 0) {
                return -1;
            }
        }
        return 0;
    } else {
        count = (D_8009DEE8 - D_8009DEEC) & 0x3F;
        if (count != 0) {
            func_80094CEC();
        }
        if ((*D_8009DED4 & 0x1000000) || !(*(volatile u32 *)D_8009DEC8 & 0x4000000)) {
            return count ? count : 1;
        }
        return count;
    }
}
