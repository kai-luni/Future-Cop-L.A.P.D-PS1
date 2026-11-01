#include "game/types.h"

struct CdlLOC {
    u8 minute;
    u8 second;
    u8 sector;
    u8 track;
};

extern struct CdlLOC *func_8008E61C(s32 sector, struct CdlLOC *location);
extern s32 func_8008E1BC(u8 com, u8 *param, u8 *result);
extern void func_80090FA0(s32 sectors, u32 *buf, s32 mode);
extern s32 func_8009113C(s32 mode, u8 *result);

s32 func_8009081C(s32 sectors, s32 sector, u32 *buf)
{
    struct CdlLOC loc;

    func_8008E61C(sector, &loc);
    func_8008E1BC(2, (u8 *)&loc, 0);
    func_80090FA0(sectors, buf, 0x80);
    return func_8009113C(0, 0) == 0;
}
