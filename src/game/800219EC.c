// Categories: Resources > Data Adjustment
#include "game/types.h"
typedef struct { u8 unk00[4]; u8 count; u8 unk05[0xB]; s32 offsets[1]; } D;

/**
 *  @brief Adds an amount to fields in records referenced by an offset table.
 *
 *  The input buffer contains a byte count and an array of offsets relative
 *  to its start; each nonzero offset identifies a counted record group.
 *  @param arg0 Base of the writable data buffer and offset table.
 *  @param amount Signed amount added to each selected record field.
 *  @par Globals
 *  None referenced directly.
 *  @par Control flow
 *  Loops over offsets, skips zero offsets, and walks each group in five-word steps.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Adds amount to the word at offset 0x14 from each successive group position.
 *  @par Callers
 *  Confirmed: func_80021CB0.
 */
void func_800219EC(u8 *arg0, s32 amount)
{
    s32 i; register s32 j __asm__("$6");
    D *data = (D *)arg0;
    s32 *offsets = data->offsets;
    for (i = 0; i < data->count; i++) {
        s32 offset = offsets[i];
        if (offset != 0) {
            u8 *entry = arg0 + offset;
            s32 *p = (s32 *)entry;
            for (j = 0; j < *entry; j++) { p[5] += amount; p += 5; }
        }
    }
}
