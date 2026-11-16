// Categories: Game Engine > State
#include "game/types.h"

typedef struct {
    u8 pad00[0x14];
    s32 count;
    u8 pad18[0x2C];
    struct {
        s32 field;
        u8 pad[0x20];
    } array[1];
} Unk800A0968;

extern Unk800A0968 D_800A0968;

/**
 *  @brief Clears a field in each counted entry of a global array.
 *
 *  Iterates over the entries described by the global count and writes zero.
 *  Other fields in the entries are left unchanged by this function.
 *  @return Always 0.
 *  @par Globals
 *  D_800A0968 supplies the count and array whose fields are cleared.
 *  @par Control flow
 *  Loops from index zero while the index is less than the count.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Sets each visited array entry's field to zero.
 *  @par Callers
 *  Confirmed: func_800226D8.
 */
s32 func_80022C68(void)
{
    s32 i;
    s32 ret;

    i = 0;
    ret = i;

    for (; i < D_800A0968.count; i++) {
        D_800A0968.array[i].field = 0;
    }

    return ret;
}
