// Categories: Resources > Data Adjustment
#include "game/types.h"
typedef struct { u8 unk00[4]; u8 count; u8 unk05[0xB]; s32 offsets[1]; } D;

void gAdjustSoundBankSampleOffsets(u8 *arg0, s32 amount) __asm__("func_800219EC");

/**
 * @brief Adds an address adjustment to selected sound bank record words.
 *
 * @param arg0 Writable bank with an offset table at +0x10.
 * @param amount Signed adjustment added to each selected word.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Reads the group count byte at +4 and the relative offsets from +0x10. Skips
 * zero offsets. For each group, uses its first byte as the record count,
 * updates the word at group +0x14, and advances by 0x14 per record.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80021CB0.
 *
 * @par Role and path
 * Bank registration applies the SPU allocation address to the bank records
 * after upload.
 *
 * @par Unknown
 * The precise on-disc field layout of the remaining group data is open.
 */
void gAdjustSoundBankSampleOffsets(u8 *arg0, s32 amount)
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
