// Categories: Platform > CD-ROM | Resources > File Access
#include "game/types.h"

extern u8 gPathTable[] __asm__("D_800A448C");
extern s32 func_8008CCA0(s32 arg0, u8 *arg1);

/**
 * @brief Searches the cached ISO-9660 path table for a directory entry matching parent ID and name.
 *
 * Walks the 44-byte-stride path table populated by func_80090218 (parent ID at 0x800A4484, NUL-terminated name at D_800A448C) up to 0x80 entries until parent ID == 0, comparing names with func_8008CCA0 when parent ID == arg0. Category Platform > CD-ROM | Resources > File Access follows the ISO-9660 path table and CD directory lookup.
 *
 * @param arg0 1-based parent directory ID to match against entry +0x04.
 * @param arg1 Pointer to NUL-terminated directory name string.
 * @return 1-based directory ID (index + 1) on match, or -1 if not found.
 *
 * @par Globals
 * D_800A4484 - parent directory ID field (+0x04 in 44-byte stride table, 0 ends search); D_800A448C - entry name field (+0x0C).
 *
 * @par Control flow
 * Loop advancing stride 0x2C up to index 0x80; breaks early when parent ID == 0 and returns i + 1 on first match.
 *
 * @par Calls
 * func_8008CCA0 - compares arg1 against entry name D_800A448C + i * 0x2C via BIOS A0:17 (strcmp).
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8008FF20 (0x80090004 in asm/us/main.s) / Likely: none / Unknown: none.
 */
s32 func_800904DC(s32 arg0, s32 arg1)
{
    s32 i;
    s32 off;
    s32 hi;
    u8 *ptr;

    i = 0;
    __asm__("" : "=r"(i) : "0"(i));
    ptr = gPathTable;
    off = 0;

    for (;;) {
        s32 entry;

        hi = 0x800A0000;
        __asm__ volatile("" : "=r"(hi) : "0"(hi));
        entry = *(s32 *)(hi + off + 0x4484);
        if (entry == 0) {
            break;
        }
        if (entry == arg0) {
            if (func_8008CCA0(arg1, ptr) == 0) {
                return i + 1;
            }
        }
        ptr += 0x2C;
        i++;
        off += 0x2C;
        if (i >= 0x80) {
            break;
        }
    }

    return -1;
}
