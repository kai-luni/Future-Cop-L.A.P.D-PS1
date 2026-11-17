// Categories: Resources > Asset Loader | Graphics > Animation
#include "game/types.h"

extern s32 gVertexBlockTable __asm__("D_8009F554");
extern s32 gNormalBlockTable __asm__("D_8009F558");
extern s32 D_8009F55C;
extern s32 D_8009F560;
extern s32 D_8009F564;
extern s32 D_8009F568;

/**
 * @brief Partitions the 0x1B18-byte buffer into four 0x400-byte 3D/animation pointer tables and resets list heads.
 *
 * Receives the zeroed D_8009FA08 buffer returned by func_80063428 and assigns 0x400-byte slices to D_8009F554 (LV4D), D_8009F558 (LN4D), D_8009F55C (LR3D), and D_8009F560 (FR3D) populated by func_80012E48, while clearing animation list heads D_8009F564 and D_8009F568.
 *
 * @param arg0 Base address of the allocated table buffer (D_8009FA08 from func_80063428).
 *
 * @par Globals
 * D_8009F554..D_8009F560 - 0x400-byte chunk pointer tables at arg0 + 0x000..+0xC00; D_8009F564/D_8009F568 - active animation node/wave list heads cleared to 0.
 *
 * @par Control flow
 * Straight-line pointer arithmetic and six global stores; no branches or loops.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Sets D_8009F554..D_8009F560 to arg0 + 0x000/0x400/0x800/0xC00 and clears D_8009F564 and D_8009F568.
 *
 * @par Callers
 * Confirmed: func_80024A24 (0x80024C5C, receives func_80063428() return during mission boot) / Likely: none / Unknown: none.
 */
void func_80012E1C(s32 arg0)
{
    register s32 next __asm__("$2");

    next = arg0 + 0x400;
    gNormalBlockTable = next;
    next = arg0 + 0x800;
    D_8009F55C = next;
    next = arg0 + 0xC00;
    gVertexBlockTable = arg0;
    D_8009F560 = next;
    D_8009F564 = 0;
    D_8009F568 = 0;
}
