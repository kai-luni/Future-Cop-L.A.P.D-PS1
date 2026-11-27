// Categories: Platform > CD-ROM | Utilities > Time Conversion
#include "game/types.h"

struct CdlLOC {
    u8 minute;
    u8 second;
    u8 sector;
    u8 track;
};
struct CdlLOC *gCdIntToPos(s32 sector, struct CdlLOC *location) __asm__("func_8008E61C");


/**
 * @brief Converts a zero-based logical CD sector number into a BCD-encoded MSF struct CdlLOC.
 *
 * Confirmed: adds the 150-sector (2-second) pre-gap to sector and converts sector % 75, (sector / 75) % 60, and (sector / 75) / 60 into packed BCD bytes stored in location->sector, location->second, and location->minute; leaves location->track untouched and returns location.
 * Category follows struct CdlLOC MSF layout and CD-ROM callers func_800889DC, func_80090580, and func_8009081C (before CdlSetloc command 2).
 * Likely: PSX libcd CdIntToPos routine translating linear LBA sectors to minute/second/sector BCD coordinates; Unknown: none.
 *
 * @param sector Logical zero-based CD sector index (150 is added internally before splitting into 75 sectors/sec and 60 sec/min).
 * @param location Output struct CdlLOC receiving BCD minute (+0), second (+1), and sector (+2).
 * @return The location pointer passed in.
 *
 * @par Globals
 * None.
 * @par Control flow
 * Straight-line arithmetic using an inlined encode_bcd helper ((v / 10) << 4) + (v % 10); no loops or branches.
 * @par Calls
 * None (encode_bcd is static/inlined).
 * @par Side effects
 * Writes minute, second, and sector bytes of *location.
 * @par Callers
 * Confirmed: func_800889DC (0x800889FC), func_80090580 (0x8009068C), func_8009081C (0x80090838) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Adds the 150-sector pre-gap to sector and encodes minute, second (75
 * sectors/sec), and sector as packed BCD bytes into location, leaving
 * location->track untouched and returning location. Accesses no globals and
 * is called by func_800889DC, func_80090580, and func_8009081C.
 */
struct CdlLOC *gCdIntToPos(s32 sector, struct CdlLOC *location)
{
    inline s32 encode_bcd(s32 value)
    {
        return ((value / 10) << 4) + (value % 10);
    }

    sector += 150;
    location->sector = encode_bcd(sector % 75);
    location->second = encode_bcd((sector / 75) % 60);
    location->minute = encode_bcd((sector / 75) / 60);
    return location;
}
