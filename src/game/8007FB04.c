#include "game/types.h"

extern u8 *D_8009F92C;
extern u8 *D_8009F468;
extern s32 D_8009F464;
extern u8 D_8009F8BD;
extern u8 D_8009F8BE;
extern u8 D_8009F8C0;
extern u8 D_800A36E8[];
extern u8 D_800A36B8[];
extern u8 D_800A3718[];
extern u8 D_800120A4[];

extern s32 func_800207B4();
extern void func_80078740(void);
extern s32 func_8001F37C(void);
extern void func_80080B70(void);
extern void func_8008204C(void);
extern void func_80081204(void);
extern s32 func_8007F750(void);
extern void func_80080010(void);
extern void func_80080AD4(void);
extern s32 func_80020C90(u8 *arg0);
extern s32 func_80020E68(s32 arg0);
extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern void func_80078650(u8 *arg0, s32 arg1);
extern void func_80082020(void);

s32 func_8007FB04(void)
{
    register s32 state __asm__("$18");
    register u32 s0 __asm__("$16");
    register u32 s1 __asm__("$17");

    state = func_800207B4(0);
    if (state != *(s32 *)(D_8009F92C + 0x208)) {
        if (D_8009F468 != 0 &&
            D_8009F468 != D_800A36E8 &&
            D_8009F468 != D_800A36B8) {
            func_80078740();
        }

        D_8009F92C[0x203] = D_8009F92C[0x200];
        D_8009F92C[0x204] = D_8009F92C[0x201];

        if (state >= 0) {
            D_8009F92C[0x200] = 2;
            D_8009F92C[0x201] = 0;
            D_8009F92C[0x202] = 0;
            s0 = 1;
            D_8009F8BD = s0;
            D_8009F8BE = s0;
            s1 = D_8009F92C[0x203];

            if (s1 == 2 && (u32)(D_8009F92C[0x204] - 1) < 3) {
                if (func_8001F37C() != 0) {
                    if (*(s8 *)(D_8009F92C + 0x922) != 0) {
                        D_8009F92C[0x922] = D_8009F92C[0x922] - 1;
                        if (D_8009F92C[0x204] == s1) {
                            func_80080B70();
                        } else if (D_8009F92C[0x204] == 3) {
                            func_8008204C();
                        } else {
                            func_80081204();
                        }
                    } else {
                        D_8009F92C[0x202] = 3;
                    }
                } else if ((u32)(D_8009F92C[0x204] - 1) < 2) {
                    D_8009F92C[0x202] = s1;
                    D_8009F92C[0x205] = s0;
                    s0 = D_8009F92C[0x204];
                    if (s0 == 1) {
                        if (func_8007F750() != 0) {
                            func_80080010();
                            if (*(s32 *)(D_8009F92C + 0x40C) != s0) {
                                D_8009F92C[0x205] = 3;
                                *(s16 *)(D_8009F92C + 0x942) = 0x37D2;
                            }
                            if (D_8009F464 == s1) {
                                func_80080AD4();
                                D_8009F464 = 3;
                            }
                        } else {
                            D_8009F92C[0x205] = s1;
                            D_8009F8BD = 0;
                        }
                    } else {
                        *(s16 *)(D_8009F92C + 0x942) = 0x37A0;
                    }
                }
            }

            if (D_8009F92C[0x202] == 0) {
                if (func_80020C90(D_800120A4) != 0) {
                    func_80081204();
                } else if (func_80020E68(0x678) == 0) {
                    D_8009F8BD = 0;
                    D_8009F92C[0x206] = 1;
                } else {
                    func_8001E4C0(D_8009F92C + 0x20C, 0, 0x678);
                    func_80080010();
                    if (D_8009F92C[0x921] == 0) {
                        D_8009F8BD = 0;
                    }
                }
            }
        } else {
            D_8009F92C[0x205] = 0;
            D_8009F92C[0x206] = 0;

            switch (state) {
            default:
                D_8009F92C[0x200] = 0;
                D_8009F92C[0x201] = 0;
                D_8009F92C[0x207] = 0;
                D_8009F8BD = 0;
                D_8009F8BE = 1;
                *(s16 *)(D_8009F92C + 0x942) = 0;
                func_80082020();
                __asm__ volatile ("");
                break;
            case -1:
                D_8009F92C[0x200] = 3;
                D_8009F92C[0x202] = 0;
                D_8009F92C[0x201] = 0;
                D_8009F92C[0x207] = 0;
                func_80078650(D_800A3718, 1);
                D_8009F468[0] = 1;
                D_8009F8BD = 1;
                D_8009F8BE = 1;
                *(s16 *)(D_8009F92C + 0x942) = 0;
                func_80082020();
                __asm__ volatile ("");
                break;
            case -2:
                D_8009F92C[0x200] = 1;
                D_8009F92C[0x202] = 0;
                D_8009F92C[0x201] = 0;
                D_8009F92C[0x207] = 0;
                D_8009F8BD = 0;
                D_8009F8BE = 0;
                *(s16 *)(D_8009F92C + 0x942) = 0;
                func_80082020();
                __asm__ volatile ("");
                break;
            case -4:
                D_8009F92C[0x200] = 2;
                D_8009F92C[0x201] = 1;
                D_8009F92C[0x207] = 0;
                D_8009F8BD = 0;
                D_8009F8BE = 0;
                *(s16 *)(D_8009F92C + 0x942) = 0;
                func_80082020();
                __asm__ volatile ("");
                break;
            case -5:
                D_8009F92C[0x200] = 2;
                if (D_8009F92C[0x207] != 0) {
                    D_8009F92C[0x201] = 3;
                } else {
                    D_8009F92C[0x201] = 2;
                }
                D_8009F92C[0x207] = 0;
                D_8009F8BD = 0;
                D_8009F8BE = 0;
                *(s16 *)(D_8009F92C + 0x942) = 0;
                break;
            case -6:
                D_8009F92C[0x200] = 3;
                D_8009F92C[0x202] = 0;
                D_8009F92C[0x201] = 3;
                D_8009F92C[0x207] = 1;
                D_8009F8BD = 0;
                D_8009F8BE = 0;
                func_80082020();
                *(s16 *)(D_8009F92C + 0x942) = 0;
                break;
            }
            D_8009F8C0 = 0;
        }
        *(s32 *)(D_8009F92C + 0x208) = state;
    }
    return state;
}
