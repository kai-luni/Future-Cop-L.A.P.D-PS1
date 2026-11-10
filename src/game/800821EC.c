#include "game/types.h"

struct State821EC {
    u8 pad0[0x200];
    u8 mode;
    u8 kind;
    u8 pad202[5];
    u8 flag;
    u8 pad208[0x73a];
    u16 value;
};

extern struct State821EC *D_8009F92C;
extern u8 D_8009F8BD;
extern u8 D_8009F8BE;

void func_800821EC(void)
{
    if (D_8009F92C->mode == 3) {
        D_8009F92C->mode = 4;
        if (D_8009F92C->flag) {
            D_8009F8BD = 0;
            D_8009F8BE = 0;
        } else {
            D_8009F8BD = 0;
            D_8009F8BE = 1;
        }
    } else if (D_8009F92C->mode == 5) {
        D_8009F92C->mode = 2;
        if ((unsigned)(D_8009F92C->kind - 2) >= 2) {
            D_8009F92C->value = 0x385e;
        }
    }
}
