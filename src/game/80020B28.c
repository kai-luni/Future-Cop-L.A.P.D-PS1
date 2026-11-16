// Categories: Platform > Memory Card | Resources > Save Data
#include "../../include/game/types.h"

struct TableRecord {
    u8 bytes[4];
    u8 pad_04[4];
    u16 value;
    u8 pad_0A[0x76];
};

/**
 * @brief Initializes the 20 broken-sector replacement slots (indices 16..35) in a card image.
 *
 * Iterates over 0x80-byte directory/sector frames 0x10 through 0x23 and sets each record's
 * 4-byte sector tag (+0x00..+0x03) to 0xFF and its 16-bit link field (+0x08) to 0xFFFF.
 *
 * @param records Pointer to the 36-frame (0x1200-byte) PS1 memory-card image buffer.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Single counted for-loop from index 0x10 to 0x23 (20 records of stride 0x80).
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes 0xFF bytes at +0x00..+0x03 and 0xFFFF at +0x08 in records[16..35].
 *
 * @par Callers
 * Confirmed: func_80020B64 (0x80020C24, when building an empty formatted memory-card image) / Likely: none / Unknown: none.
 */
void func_80020B28(struct TableRecord *records)
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
