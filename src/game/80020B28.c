// Categories: Platform > Memory Card | Resources > Save Data
#include "../../include/game/types.h"

struct TableRecord {
    u8 bytes[4];
    u8 pad_04[4];
    u16 value;
    u8 pad_0A[0x76];
};

void gInitCardReplacementFrames(struct TableRecord *records) __asm__("func_80020B28");

/**
 * @brief Initializes the 20 replacement-sector records in a blank memory-card
 * system area.
 *
 * @param records Base of a writable card system-area image with space for 36
 *     frames.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Starts at +0x800 and steps 0x80 bytes per record for indices 16 through 35.
 * Stores 0xFF into bytes 0 through 3 and 0xFFFF at +8.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gPrepareCardFormat.
 *
 * @par Role and path
 * The card format builder uses this helper after writing the directory frames.
 *
 * @par Unknown
 * The routine initializes only these replacement-sector fields.
 */
void gInitCardReplacementFrames(struct TableRecord *records)
{
    u32 index;

    for (index = 0x10; index < 0x24; index++) {
        records[index].bytes[0] = 0xFF;
        records[index].bytes[1] = 0xFF;
        records[index].bytes[2] = 0xFF;
        records[index].bytes[3] = 0xFF;
        records[index].value = 0xFFFF;
    }
}
