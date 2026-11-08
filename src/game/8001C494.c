#include "game/types.h"

struct CdlLOC {
    u8 minute;
    u8 second;
    u8 sector;
    u8 track;
};

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
