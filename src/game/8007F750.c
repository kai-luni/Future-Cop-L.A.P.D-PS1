// Categories: Platform > Save Data
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

/**
 *  @brief Checks the save header and all ten save entries.
 *
 *  The header check sets the save's validity byte, while the return value
 *  also reflects whether every entry passed its own check.
 *  @return 1 if the header and all entries pass, otherwise 0.
 *  @par Globals
 *  D_8009F92C points to the save containing the checked data and validity byte.
 *  @par Control flow
 *  Checks the header once, then checks all ten entries even after a failure.
 *  @par Calls
 *  func_8007F824 checks each data region against its stored values, with
 *  clear set to 0 for the header and 1 for entries.
 *  @par Side effects
 *  Sets the header validity byte and passes entry data to the checker with clear enabled.
 *  @par Callers
 *  Confirmed: func_8007FB04.
 */
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
