#include "game/types.h"

struct Entry {
    u8 data[0x68];
    u32 sum;
    u32 other;
};

struct Save {
    u8 pad[0x40C];
    u8 header[0x10];
    u32 sum;
    u32 other;
    struct Entry entries[10];
    u8 pad2[0x923 - 0x424 - 10 * 0x70];
    u8 valid;
};

extern struct Save *D_8009F92C;
extern s32 func_8007F824(u8 *data, u32 len, u32 expectedSum, u32 expectedOther, s32 clear);

s32 func_8007F750(void)
{
    s32 ok;
    u32 i;

    ok = 1;
    if (func_8007F824(D_8009F92C->header, 0x10, D_8009F92C->sum, D_8009F92C->other, 0) != 0) {
        D_8009F92C->valid = ok;
    } else {
        ok = 0;
        D_8009F92C->valid = 0;
    }
    for (i = 0; i < 10; i++) {
        if (func_8007F824(D_8009F92C->entries[i].data, 0x68, D_8009F92C->entries[i].sum,
                          D_8009F92C->entries[i].other, 1) == 0) {
            ok = 0;
        }
    }
    return ok;
}
