#include "game/types.h"

typedef struct {
    u8 unk00[8];
    s32 value;
} D_8009BEF4_Entry;

extern u8 D_8009FA98;
extern u8 D_8009F8D8;
extern u8 D_8009F8D9;
extern u8 D_8009F900;
extern u8 D_8009F901;
extern u8 D_8009F902;
extern u8 D_8009F903;
extern s8 D_8009F904;
extern u8 D_8009BFD4[];
extern D_8009BEF4_Entry D_8009BEF4[];
extern u8 D_800AAC50[];
extern u8 D_800A374D[];

extern u8 func_8001EB2C(s32 arg0);

void func_8007BFCC(s32 port, s32 value)
{
    extern u8 D_8009FA99[];
    s32 clampedPort;
    s32 clampedValue;
    u8 *dst;
    u8 *src;
    s32 i;
    u8 *base;

    if (port >= 0) {
        if (port < 2) {
            clampedPort = port;
        } else {
            clampedPort = 1;
        }
    } else {
        clampedPort = 0;
    }
    port = clampedPort;

    if (value >= 0) {
        if (value < 4) {
            clampedValue = value;
        } else {
            clampedValue = 3;
        }
    } else {
        clampedValue = 0;
    }
    value = clampedValue;

    ((u8 *)D_8009FA99 - 1)[port] = value;
    src = D_8009BFD4 + value * 14;
    i = 0;
    base = D_800AAC50;
    dst = base + port * 56;
    while (i < 14) {
        *(s32 *)(dst + src[i] * 4) = D_8009BEF4[i].value;
        i++;
    }
}

