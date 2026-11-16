// Categories: Utilities > Numeric Conversion
#include "game/types.h"

struct Unk607B8 {
    u8 unk00[6];
    s16 unk06;
    s16 unk08;
    u8 unk0A[2];
    s16 unk0C;
};

/**
 *  @brief Rescales three signed fields in place.
 *
 *  Applies integer divisions and shifts to the fields at offsets 6, 8,
 *  and 12, leaving the middle field unchanged when it is zero.
 *  @param value Structure containing the fields to rescale.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  Conditionally rescales unk08; the other two assignments are unconditional.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites value's unk06, unk08 when nonzero, and unk0C fields.
 *  @par Callers
 *  Confirmed: func_80060838, func_8006A898.
 */
void func_800607B8(struct Unk607B8 *value)
{
    value->unk06 = value->unk06 / 300;
    if (value->unk08 != 0) {
        value->unk08 = (value->unk08 << 8) / 300;
    }
    value->unk0C = (value->unk0C << 12) / 218;
}
