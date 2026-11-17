// Categories: Platform > Event Dispatch
#include "game/cdrom_globals.h"

/*
 * func_8008FE28 (0x8008FE28, 0xD8 bytes) -- EXACT reconstruction.
 *
 * profile: gcc-2.7.2-psx, cc1 "-O2 -G0", maspsx ["--passthrough"], link: true with
 *          D_8009CB84 / D_800A3E58 / D_800A3E50 as linker symbols (so the assembler
 *          keeps %hi/%lo relocations: "lui 0x800a; addiu -13435" instead of
 *          "lui 0x8009; ori 0xcb85").
 *
 * Notes:
 *  - retail materialises 0x8009CB85 directly in $s1 and derives $s3 = $s1 - 1, so the
 *    base pointer is the 0x8009CB84+1 address and the second access is p[-1].
 *  - --passthrough keeps .set reorder: GNU as then splits "la $5,<str>" as
 *    "lui $5,%hi" before / "addiu $5,$5,%lo" inside the "jalr" delay slot and moves
 *    "addu $sp,$sp,40" into the delay slot of "jr $ra", exactly like the retail aspsx.
 *  - the loop exit value is copied with the `move` pseudo, which aspsx/maspsx expand to
 *    "addu", GNU as expands to "or"; the addu is spelled out explicitly.
 */
typedef unsigned char u8;
typedef unsigned int u32;
typedef signed int s32;

extern u8 *gCdromIndexStatusPtr __asm__("D_8009CB6C");
extern u8 gCdStateBytes[] __asm__("D_8009CB84");
extern u32 D_8009C8AC, D_8009C8A8;
extern char D_800A3E58[];
extern char gStatusBuffer8[] __asm__("D_800A3E50");
extern s32 func_8008E7A0(void);

/**
 *  @brief Drains event results and invokes callbacks for selected result bits.
 *
 *  The low two bits of the mode byte are saved before polling and written
 *  back after the event source returns zero.
 *  @par Globals
 *  D_8009CB6C points to the mode byte; D_8009CB84 supplies callback bytes.
 *  D_8009C8AC and D_8009C8A8 hold optional callbacks; D_800A3E58 and
 *  D_800A3E50 supply their respective string arguments.
 *  @par Control flow
 *  Polls until a zero result; bit 4 dispatches the first callback and bit 2
 *  dispatches the second, each only when its callback pointer is nonzero.
 *  @par Calls
 *  func_8008E7A0 supplies each event result; configured callbacks receive
 *  the corresponding byte and string.
 *  @par Side effects
 *  Invokes callbacks and replaces the mode byte with its saved low two bits.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8008FE28(void)
{
    u8 mode = *gCdromIndexStatusPtr & 3;
    u8 *p = gCdStateBytes + 1;
    u8 *q = p - 1;
    s32 r;

    for (;;) {
        func_8008E7A0();
        __asm__ volatile("addu %0,$2,$0" : "=r"(r));
        if (r == 0)
            break;
        if ((r & 4) && D_8009C8AC != 0)
            ((void (*)(u8, char *))D_8009C8AC)(p[0], D_800A3E58);
        if ((r & 2) && D_8009C8A8 != 0)
            ((void (*)(u8, char *))D_8009C8A8)(q[0], gStatusBuffer8);
    }
    *gCdromIndexStatusPtr = mode;
}
