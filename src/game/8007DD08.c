typedef signed char s8;
typedef unsigned char u8;
typedef int s32;

struct Entry {
    unsigned int unknown;
    u8 *values;
    u8 unused;
    u8 replacement;
    u8 limit;
    u8 pad[9];
};
extern struct Entry D_8009C0A8[];
extern s8 *D_8009F468;
extern u8 D_800A6FE0[];
extern s8 D_8009F924[];
extern s8 D_8009F926[];
extern s8 D_8009F928[];
extern u8 D_8009C030[];
extern u8 D_8009C050[];
extern u8 *D_8009C0D0;
extern s32 D_8009FA60;

extern void func_8007D2DC(s32);
extern void func_800788F0(u8 *, s32, s32, s32, s32, s32, s32, s32);
extern void func_80078B34(s32, s32, s32);
extern void func_8007D7CC(u8 *, s32, s32, s32, s32, s32);

void func_8007DD08(s32 arg0)
{
    s32 s3 = 0;
    s32 s6 = 0;
    s32 s5;
    s32 s0;
    s32 s4;
    s32 fp = D_8009F468[arg0];
    s32 i3;

    func_8007D2DC(s3);
    s5 = D_8009F468[5];
    s4 = fp - 1;
    if (s5 == 2) {
        func_800788F0(D_800A6FE0, 0x23, 0x35, 0x10, 0, 1, 8, s5);
        func_800788F0((u8 *)s3, 0x23, 0x3F, 0x10, 10000, 1, 8, s5);
        func_800788F0(D_800A6FE0 + 9, 0xA4, 0x35, 0x10, 0, 1, 8, s5);
        func_800788F0((u8 *)s6, 0xA4, 0x3F, 0x10, 10000, 1, 8, s5);
        s0 = 0xA4;
        if (arg0 != 0) {
            s4 = fp - 5;
            s3 = 2;
            if (D_8009F468[arg0] == 4)
                s6 = 1;
            else if (D_8009C0A8[s4].replacement != D_8009C0A8[s4].limit)
                s6 = 2;
        } else {
            s0 = 0x25;
            s3 = 1;
            if (D_8009F468[0] == 0)
                s6 = 1;
            else if (D_8009C0A8[s4].replacement != D_8009C0A8[s4].limit)
                s6 = 2;
        }
        func_8007D7CC(D_8009C030 + D_8009F924[arg0] * 6, s0, 0x4F, s4 == 0, 0, 0);
        func_8007D7CC(D_8009C050 + D_8009F926[arg0] * 6, s0, 0x71, s4 == 1, 1, 0);
        i3 = D_8009F928[arg0];
        func_8007D7CC(D_8009C0D0 + i3 * 6, s0, 0x93, s4 == 2, 2, 0);
    } else {
        if (D_8009F468[arg0] == 0)
            s6 = 1;
        else if (D_8009C0A8[s4].replacement != D_8009C0A8[s4].limit)
            s6 = 2;
        func_800788F0(D_800A6FE0, 0x53, 0x3C, 0x10, 0, 4, 8, 2);
        func_800788F0(0, 0x53, 0x44, 0x10, 10000, 4, 8, 2);
        func_8007D7CC(D_8009C030 + D_8009F924[arg0] * 6, 0x86, 0x35, s4 == 0, 0, 1);
        func_8007D7CC(D_8009C050 + D_8009F926[arg0] * 6, 0x86, 0x5F, s4 == 1, 1, 1);
        i3 = D_8009F928[arg0];
        func_8007D7CC(D_8009C0D0 + i3 * 6, 0x86, 0x89, s4 == 2, 2, 1);
    }
    func_80078B34(0x2968, 0, s3);
    if (s4 >= 0 && D_8009C0A8[s4].values[arg0] > D_8009C0A8[s4].limit)
        func_80078B34(0x521C, 0, s3);
    func_80078B34(0x10CC, 1, s3);
    if ((D_8009FA60 >> 9) & 1) {
        switch (s6) {
        case 1: func_80078B34(0x297C, 1, s3); break;
        case 2: func_80078B34(0x2972, 1, s3); break;
        }
    }
}
