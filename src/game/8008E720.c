// Categories: Platform > CD-ROM | Utilities > Time Conversion
#include "game/types.h"

struct CdlLOC {
    u8 minute;
    u8 second;
    u8 sector;
    u8 track;
};

#define BCD_TO_INT(value) (((value) >> 4) * 10 + ((value) & 0xF))
s32 gCdPosToInt(struct CdlLOC *location) __asm__("func_8008E720");


/**
 * @brief Converts a CdlLOC position back into a logical sector number.
 *
 * The inverse of CD-ROM MSF encoding: minute and second are BCD-decoded,
 * combined into seconds, scaled by 75 and the sector field is added; the result
 * is the track-relative sector because the conventional 150-sector lead-in is
 * subtracted at the end.
 * @param location CdlLOC with BCD minute, second and sector bytes.
 * @return The logical sector number.
 * @par Globals
 * None.
 * @par Control flow
 * Straight-line arithmetic.
 * @par Calls
 * None.
 * @par Side effects
 * None observed.
 * @par Callers
 * Confirmed: the CD file service (func_8008FC30 neighbourhood).
 *
 * @par Role and path
 * Decodes BCD minute, second, and sector bytes from location into a linear
 * sector count and subtracts 150 pre-gap sectors. Performs no function calls,
 * accesses no globals, and has no direct callers in src/.
 */
s32 gCdPosToInt(struct CdlLOC *location)
{
    return (BCD_TO_INT(location->minute) * 60
            + BCD_TO_INT(location->second)) * 75
        + BCD_TO_INT(location->sector) - 150;
}
