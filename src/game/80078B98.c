#include "game/types.h"

struct Entry78B98 { u16 text; u16 x; };
extern struct Entry78B98 D_800A3578[2][2];
extern void func_800788F0(u8 *, s32, s32, s32, s32, s32, s32, s32);

void func_80078B98(void)
{
    unsigned int i, j;
    register int font asm("$21") = 2;
    int y = 0xC7;


    i = 0;
    do {


        for (j = 0; j < 2; j++) {
            if (D_800A3578[i][j].text != 0) {
                if (D_800A3578[i][j].text == 0x521C) {
                    func_800788F0(0, D_800A3578[i][j].x, y, 12,
                                  D_800A3578[i][j].text, 4, 8, font);
                } else {
                    func_800788F0(0, D_800A3578[i][j].x, y, 16,
                                  D_800A3578[i][j].text, 4, 8, font);
                }
            }
        }
        y += 11;
    } while (++i < 2);
}
