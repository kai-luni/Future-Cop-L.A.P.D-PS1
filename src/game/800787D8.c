#include "game/types.h"

typedef struct Entry {
    u8 pad00[0xE];
    u8 unk0E;
    s8 unk0F;
    u8 pad10[0xC];
} Entry;

extern u8 *D_8009F468;

void func_800787D8(s8 arg0, s8 arg1, s8 arg2)
{
    Entry *entries;
    s8 i;

    if (D_8009F468 == 0) {
        return;
    }

    i = 0;
    while (1) {
        if (arg0 >= (s8)D_8009F468[2]) {
            arg0 = 0;
        }
        if (arg0 < 0) {
            arg0 = D_8009F468[2] - 1;
        }

        entries = *(Entry **)(D_8009F468 + 0x28);
        if (entries[arg0].unk0F == arg2 && entries[arg0].unk0E != 0) {
            D_8009F468[entries[arg0].unk0F] = arg0;
            return;
        }

        if (arg1 == 0 || i > (s8)D_8009F468[2]) {
            return;
        }

        i++;
        if (arg1 > 0) {
            arg0++;
        } else {
            arg0--;
        }
    }
}
