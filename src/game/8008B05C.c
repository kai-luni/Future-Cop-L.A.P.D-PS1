// Categories: Input > DualShock | Platform > Controller
#include "../../include/game/types.h"

struct BufferShape {
    u8 pad_00[0xE3];
    u8 width;
    u8 pad_E4[5];
    u8 height;
    u8 pad_EA[2];
    u32 data;
};

/**
 * @brief Computes the required aligned byte size for a controller port handshake table block.
 *
 * Confirmed: reads u8 width (+0xE3, mode_count in func_8008AF10), u8 height (+0xE9, actuator_count), and u32 data (+0xEC, combination_storage_bytes), returning (((width + 1) >> 1) << 2) + (((height * 5 + 3) & 0xFFC) + 4) + data.
 * Category follows PadPort table layout verified in func_8008AF10 and func_8008B094, which rejects blocks >= 0x81 bytes before carving mode/capability/combination tables at port+0x63.
 * Likely: buffer-bounds helper accounting for 4-byte alignment slack (+4 bytes) when packing pad capability tables into the 0xF0-byte port state; Unknown: none.
 *
 * @param shape Pointer to the PadPort / BufferShape descriptor (+0xE3, +0xE9, +0xEC read).
 * @return Total byte size required for the mode table, capability array, alignment slack, and combination storage.
 *
 * @par Globals
 * None.
 * @par Control flow
 * Straight-line arithmetic leaf function; no branches or loops.
 * @par Calls
 * None.
 * @par Side effects
 * None observed.
 * @par Callers
 * Confirmed: func_8008AF10 (0x8008B004 in asm/us/main.s, checked against >= 0x81 before calling func_8008B094) / Likely: none / Unknown: none.
 */
s32 func_8008B05C(struct BufferShape *shape)
{
    s32 width;
    s32 height;
    s32 response_bytes;
    s32 base;
    s32 raw_width;
    s32 raw_height;

    raw_width = shape->width;
    raw_height = shape->height;
    base = shape->data;

    width = raw_width + 1;
    width >>= 1;
    width <<= 2;

    response_bytes = (raw_height << 2) + raw_height;
    height = (response_bytes + 3) & 0xFFC;
    height += 4;

    width += height;
    return width + base;
}
