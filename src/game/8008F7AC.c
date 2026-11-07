#include "game/types.h"

struct State {
    u8 pad[0x180];
    u16 a, b;
    u8 pad2[0x26];
    u16 c;
    u8 pad3[4];
    u16 d, e;
    u8 pad4[4];
    u16 f, g;
};
extern struct State *D_8009CB80;
extern u8 *D_8009CB6C, *D_8009CB70, *D_8009CB74, *D_8009CB78;

s32 func_8008F7AC(void)
{
    volatile u8 data[4];
    if (D_8009CB80->f == 0 && D_8009CB80->g == 0) {
        D_8009CB80->a = 0x3fff;
        D_8009CB80->b = 0x3fff;
    }
    D_8009CB80->d = 0x3fff;
    D_8009CB80->e = 0x3fff;
    D_8009CB80->c = 0xc001;
    data[2] = 0x80;
    data[0] = 0x80;
    data[3] = 0;
    data[1] = 0;
    *D_8009CB6C = 2;
    *D_8009CB74 = data[0];
    *D_8009CB78 = data[1];
    *D_8009CB6C = 3;
    *D_8009CB70 = data[2];
    *D_8009CB74 = data[3];
    *D_8009CB78 = 0x20;
    return 0;
}
