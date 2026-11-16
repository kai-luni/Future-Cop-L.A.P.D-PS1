// Categories: Resources > File Access > Module Names | Game Engine > Screen Loading
#include "game/types.h"

extern s32 D_8009C1CC[5];
extern s32 D_8009F3D8;
extern s32 D_8009F3E0;

extern s32 func_8008CCA0(s32 arg0, s32 arg1);

/**
 * @brief Resolves which of the two loader directory prefixes a module name belongs
 *        to.
 *
 * Compares the name against the five entries of D_8009C1CC with the BIOS strcmp.
 * Those five are the "specialist" modules that live under one directory; every
 * other name - the fall-out after all five comparisons - gets the other. The result
 * is the prefix argument of the "%s%s" format D_8009F2E4 that builds the real path
 * in func_80024E3C and func_80025370.
 *
 * @param arg0 Module name as it appears in the screen table, e.g. "FE.mis".
 * @return D_8009F3E0 ("PA\", bytes 50 41 5C 00) when the name is one of the five,
 *         otherwise D_8009F3D8 ("CW\", bytes 43 57 5C 00).
 *
 * @par Globals
 * D_8009C1CC - the five name pointers.
 * D_8009F3D8 - pointer word holding 0x8009F3D4, the string "CW\".
 * D_8009F3E0 - pointer word holding 0x8009F3DC, the string "PA\".
 *
 * @par Control flow
 * Counted loop over 0..4 that breaks on equality; a single compare of the counter
 * against 5 selects which string pointer is returned.
 *
 * @par Calls
 * func_8008CCA0 - BIOS strcmp (A(0x17)) per candidate; only the equality result is
 * used.
 *
 * @par Side effects
 * None observed: every call only reads the name.
 *
 * @par Callers
 * Confirmed: func_80024E3C (every module start goes through it).
 *
 * @par Note
 * The mapping is the reverse of what an earlier draft of func_80024E3C assumed: the
 * five listed names take "PA\", all others "CW\".
 */
s32 func_800833A8(s32 arg0)
{
    u32 i;

    for (i = 0; i < 5; i++) {
        if (func_8008CCA0(arg0, D_8009C1CC[i]) == 0) {
            break;
        }
    }

    if (i == 5) {
        return D_8009F3D8;
    }
    return D_8009F3E0;
}
