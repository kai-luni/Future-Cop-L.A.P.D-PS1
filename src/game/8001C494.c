// Categories: Platform > CD-ROM
#include "game/types.h"

struct CdlLOC {
    u8 minute;
    u8 second;
    u8 sector;
    u8 track;
};

/**
 * @brief Converts a byte offset into a CD-ROM CdlLOC position.
 *
 * The offset is shifted right by 11 to get the 2048-byte sector number, then
 * divided into seconds (75 sectors) and minutes (60 seconds). Minute, second
 * and frame are BCD-encoded and written into the CdlLOC, whose track byte is
 * cleared. The low 11 bits of the input are returned, which is the byte
 * position inside the sector.
 *
 * @param loc Destination CdlLOC (minute, second, sector, track).
 * @param value Byte offset into the disc image.
 * @return The offset inside the 2048-byte sector (value & 0x7FF).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line arithmetic with a local BCD helper.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes the four bytes of loc.
 *
 * @par Callers
 * Confirmed: func_8001C718, func_8001CE54. Likely: func_8001D0A0.
 */
u32 func_8001C494(struct CdlLOC *loc, u32 value)
{
    inline u32 encode_bcd(u32 v)
    {
        return ((u8)(v / 10) << 4) + (v % 10);
    }
    u32 sector = value >> 11;
    u32 seconds = sector / 75;
    u8 minute = seconds / 60;
    u8 second = seconds % 60;
    u8 frame = sector % 75;

    loc->minute = encode_bcd(minute);
    loc->second = encode_bcd(second);
    loc->sector = encode_bcd(frame);
    loc->track = 0;

    return value & 0x7FF;
}
