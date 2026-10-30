#include "game/types.h"

extern s16 D_8009F65C[];
extern s16 D_8009FA44[];
extern s32 D_8009F9FC;
extern s32 D_8009FA00;

extern void func_80026E9C(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

void func_80026F68(s32 arg0, s32 arg1)
{
    s32 temp;

    if (D_8009F65C[arg0] != 0) {
        if (D_8009F65C[arg0] >= 0x384) {
            D_8009FA00 = 6;
        } else {
            D_8009F65C[arg0] += D_8009F9FC;
        }
    } else if (D_8009FA44[arg0] != 0) {
        temp = D_8009FA44[arg0] - (D_8009F9FC + D_8009F9FC / 2);
        if (temp <= 0) {
            temp = 0;
        }
        D_8009FA44[arg0] = temp;
        if (temp > 0) {
            if (temp >= 0x100) {
                temp = 0xFF;
            }
            func_80026E9C(temp, temp, temp, (arg1 & 0xFF) ? 0x140 : 0xA0);
        }
    }
}
