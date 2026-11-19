// Categories: Graphics > HUD > Status Line | Graphics > 2D Drawing > Text
#include "game/types.h"

struct Entry78B98 { u16 text; u16 x; };
extern struct Entry78B98 gSlotRecordTable[2][2] __asm__("D_800A3578");
extern void func_800788F0(u8 *, s32, s32, s32, s32, s32, s32, s32);
void gDrawSlotStatusLine(void) __asm__("func_80078B98");


/**
 * @brief Draws the two queued display text slots as the bottom status lines.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Walks the 2x2 slot array D_800A3578 (the records func_80078B34 fills):
 * each non-zero text id is drawn at its stored x, y = 0xC7 for row 0 and
 * 0xD2 for row 1, alignX 4, size 8, font 2; id 0x521C uses colour 12, all
 * others colour 16.
 *
 * @par Likely role
 * The status-line pass of the HUD close func_80078DD0: the two slots carry
 * the transient messages (mission text, prompts) queued during the frame.
 *
 * @par Unknown
 * What text id 0x521C marks (the highlighted variant).
 *
 * @par Callers
 * func_80078DD0 (two call sites, verified in the retail listing; the
 * call-graph edges are missing).
 *
 * @par Environment path
 * Walks the two queued display text slots in D_800A3578 and draws each active
 * slot's status string at the bottom of the screen via func_800788F0 (called
 * by func_80078DD0:347,353).
 */
void gDrawSlotStatusLine(void)
{
    unsigned int i, j;
    register int font asm("$21") = 2;
    int y = 0xC7;


    i = 0;
    do {


        for (j = 0; j < 2; j++) {
            if (gSlotRecordTable[i][j].text != 0) {
                if (gSlotRecordTable[i][j].text == 0x521C) {
                    func_800788F0(0, gSlotRecordTable[i][j].x, y, 12,
                                  gSlotRecordTable[i][j].text, 4, 8, font);
                } else {
                    func_800788F0(0, gSlotRecordTable[i][j].x, y, 16,
                                  gSlotRecordTable[i][j].text, 4, 8, font);
                }
            }
        }
        y += 11;
    } while (++i < 2);
}
