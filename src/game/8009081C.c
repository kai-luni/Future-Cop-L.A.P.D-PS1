// Categories: Platform > CD-ROM | Resources > File Access
#include "game/types.h"

struct CdlLOC {
    u8 minute;
    u8 second;
    u8 sector;
    u8 track;
};

extern struct CdlLOC *gCdIntToPos(s32 sector, struct CdlLOC *location) __asm__("func_8008E61C");
extern s32 gCdControl(u8 com, u8 *param, u8 *result) __asm__("func_8008E1BC");
extern void gCdReadAsync(s32 sectors, u32 *buf, s32 mode) __asm__("func_80090FA0");
extern s32 gCdReadSync(s32 mode, u8 *result) __asm__("func_8009113C");
s32 gReadCdSectorsSync(s32 sectors, s32 sector, u32 *buf) __asm__("func_8009081C");


/**
 * @brief Performs a blocking CD-ROM read of `sectors` blocks starting at logical `sector` into `buf`.
 *
 * Converts `sector` to BCD `CdlLOC`, issues command 2 (`Setloc`) via func_8008E1BC, starts async read via func_80090FA0 with mode `0x80`, and blocks in func_8009113C until completion. Category Platform > CD-ROM | Resources > File Access follows CD command/DMA helpers and ISO9660 callers.
 *
 * @param sectors Number of sectors to read.
 * @param sector Logical sector index (offset by +150 frames inside func_8008E61C).
 * @param buf Destination buffer for read sector data.
 * @return `1` if func_8009113C(0, 0) returns `0` (success), `0` otherwise.
 *
 * @par Globals
 * None directly (callees access CD-ROM state D_8009CBD0..D_8009CBF8).
 *
 * @par Control flow
 * Straight-line four-call sequence returning `func_8009113C(0, 0) == 0`; no local branches or loops.
 *
 * @par Calls
 * func_8008E61C - encodes `sector` to BCD `CdlLOC`; func_8008E1BC - sends command 2 (`Setloc`) with `&loc`; func_80090FA0 - initiates `sectors` read into `buf` (mode `0x80`); func_8009113C - polls in mode `0` until settled.
 *
 * @par Side effects
 * Drives CD-ROM seek/read hardware and writes sector data into `buf`.
 *
 * @par Callers
 * Confirmed: func_80090218 (0x8009024C, 0x800902E8 in asm/us/main.s) and func_80090580 (0x800905E0 in asm/us/main.s) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Performs a blocking read of sectors starting at logical sector into buf by
 * converting sector to BCD CdlLOC with func_8008E61C, issuing CdlSetloc
 * (command 2) via func_8008E1BC, starting the read via func_80090FA0(sectors,
 * buf, 0x80), and polling func_8009113C(0, 0) until completion. Returns 1
 * when func_8009113C returns 0, or 0 otherwise. Driven by func_80090218 and
 * func_80090580.
 */
s32 gReadCdSectorsSync(s32 sectors, s32 sector, u32 *buf)
{
    struct CdlLOC loc;

    gCdIntToPos(sector, &loc);
    gCdControl(2, (u8 *)&loc, 0);
    gCdReadAsync(sectors, buf, 0x80);
    return gCdReadSync(0, 0) == 0;
}
