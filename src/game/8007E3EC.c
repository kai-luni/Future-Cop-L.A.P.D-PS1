#include "game/types.h"

extern s8 *D_8009F468;
extern s32 D_8009F8B8;
extern volatile s32 D_8009F8E0;
extern volatile s32 D_8009F8E4;
extern s32 D_8009F8E0_arr[];
extern s32 *D_8009FA94;
extern u8 D_8009F45A;
extern s8 D_8009F924[2];
extern s8 D_8009F926[2];
extern s8 D_8009F928[2];
extern u8 D_8009FA10;
extern s32 D_8009FA00;
extern u8 D_8009C030[];
extern u8 D_8009C050[];
extern u8 *D_8009C0A8[];
extern s16 D_8009B856[4];
extern s16 D_8009B7E6[4];

extern void func_8007D2DC(s32 mode);

void func_8007E3EC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4)
{
    u8 flag;
    s32 want;
    register s32 idx __asm__("$2");
    register s32 off __asm__("$3");
    want = a4;
    flag = 0;
    if (a0 != 0) {
        if (a0 != 3) {
            return;
        }
        if (D_8009F468[5] == 2) {
            idx = D_8009F8B8;
            __asm__ volatile("" ::: "memory");
            idx[D_8009F8E0_arr] = 1;
            __asm__ volatile("" ::: "memory");
            if (D_8009F8E0 != 0 && D_8009F8E4 != 0) {
                flag = 1;
                if (D_8009FA94[23] == 1) {
                    D_8009F45A = 2;
                } else {
                    D_8009F45A = 1;
                }
            }
        } else {
            flag = 1;
            if (D_8009FA94[23] == 1) {
                D_8009F45A = 2;
            } else {
                D_8009F45A = 1;
            }
        }
        if (flag != 0) {
            func_8007D2DC(1);
            D_8009FA94[10] = D_8009C030[D_8009F924[0] * 6];
            D_8009FA94[11] = D_8009C050[D_8009F926[0] * 6];
            D_8009FA94[12] = D_8009C0A8[10][D_8009F928[0] * 6];
            D_8009FA94[13] = D_8009C030[D_8009F924[1] * 6];
            D_8009FA94[14] = D_8009C050[D_8009F926[1] * 6];
            D_8009FA94[15] = D_8009C0A8[10][D_8009F928[1] * 6];
            D_8009FA10 = 0;
            D_8009FA00 = 5;
        }
    } else {
        if (D_8009F468[5] == 2) {
            s32 i = D_8009F8B8;
            if (D_8009F468[i] != want) {
                off = i << 2;
                *(s32 *)((u8 *)D_8009F8E0_arr + off) = 0;
                if (i == 1) {
                    D_8009B856[0] = 100;
                } else {
                    D_8009B7E6[0] = 100;
                }
                i = D_8009F8B8;
            }
            if (D_8009F8E0_arr[i] != 0) {
                if (i == 1) {
                    D_8009B856[0] = 110;
                } else {
                    D_8009B7E6[0] = 110;
                }
            }
        }
    }
}
