// Categories: Platform > CD-ROM | Resources > File Access
#include "game/types.h"

struct UnkD0A0 {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0C;
    s32 flags;
    s32 f14;
};

extern struct UnkD0A0 gFileSlotTable[] __asm__("D_800A6D78");
extern u8 gCdTargetLoc[] __asm__("D_8009F614");
extern u8 gCdTargetLocSecond[] __asm__("D_8009F615");
extern u8 gEventBuffer[] __asm__("D_800A0798");

extern void gByteOffsetToCdlLoc(void *a0, s32 a1) __asm__("func_8001C494");
extern s32 gCdControl(s32 a0, void *a1, void *a2) __asm__("func_8008E1BC");
s32 gSeekCdFileSlot(s32 index) __asm__("func_8001D0A0");


/**
 * @brief Seeks the CD-ROM to the current byte position of file slot `index` in D_800A6D78.
 *
 * Checks `D_800A6D78[index].flags & 0xF` (matching `1`; the `0x11` arm is unreachable after `& 0xF`), converts `D_800A6D78[index].f0C` into a BCD `CdlLOC` at D_8009F614 via func_8001C494, and issues CD-ROM command `0x15` via func_8008E1BC. Category Platform > CD-ROM | Resources > File Access follows D_800A6D78 slot seeking and CD-ROM command `0x15`.
 *
 * @param index File slot index in D_800A6D78.
 * @return `-4` if func_8008E1BC(0x15, ...) fails (`0`), otherwise `0`.
 *
 * @par Globals
 * D_800A6D78 - file slot table (`+0x0C` position, `+0x10` flags); D_8009F614/D_8009F615 - 4-byte `CdlLOC` buffer; D_800A0798 - CD command response buffer.
 *
 * @par Control flow
 * Single branch on `(flags & 0xF) == 1 || (flags & 0xF) == 0x11`, returning `-4` on CD command failure else `0`.
 *
 * @par Calls
 * func_8001C494 - encodes byte offset `f0C` into BCD `CdlLOC` at D_8009F614.
 * func_8008E1BC - executes CD-ROM command `0x15` with `D_8009F614` (`D_8009F615 - 1`) and `D_800A0798`.
 *
 * @par Side effects
 * Overwrites D_8009F614..D_8009F617 and D_800A0798 and issues a CD-ROM seek command.
 *
 * @par Callers
 * Confirmed: func_8001D6D0 (0x8001D8A0 in asm/us/main.s) and func_8001D90C (0x8001DA4C in asm/us/main.s) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Checks gFileSlotTable[index].flags & 0xF for mode 1, encodes f0C into
 * gCdTargetLoc via func_8001C494, and issues CD-ROM command 0x15 with
 * gCdTargetLoc and gEventBuffer (D_800A0798) via func_8008E1BC, returning -4
 * on failure or 0. Called by func_8001D6D0 and func_8001D90C.
 */
s32 gSeekCdFileSlot(s32 index)
{
    register u8 *a0 __asm__("$4");
    register u8 *a1 __asm__("$5");
    register u8 *a2 __asm__("$6");
    s32 t = gFileSlotTable[index].flags & 0xF;

    if (t == 1 || t == 0x11) {
        s32 r;

        a1 = (u8 *)gFileSlotTable[index].f0C;
        a0 = gCdTargetLoc;
        __asm__("" : "+r"(a0));
        gByteOffsetToCdlLoc(a0, a1);

        a0 = (u8 *)0x15;
        a1 = gCdTargetLocSecond - 1;
        a2 = gEventBuffer;
        r = gCdControl((s32)a0, a1, a2);
        if (r == 0) {
            return -4;
        }
    }
    return 0;
}
