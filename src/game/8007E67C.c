// Categories: Graphics > HUD > Layout | Graphics > 2D Drawing > Text | Graphics > Sprites > Atlas
#include "game/types.h"

typedef struct {
    u16 id;
    u16 x;
    u8 y;
    u8 align;
} Entry;

extern Entry gMenuEntryTable[] __asm__("D_8009C0E4");
extern Entry gMenuTailEntryTable[] __asm__("D_8009C18C");
extern void func_800788F0(u8 *, s32, s32, s32, s32, s32, s32, s32);
extern void func_80082AB0(s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80078B34(s32, s32, s32);

/**
 * @brief Draws the static label/sprite layout of one front-end screen and
 *        queues two display requests.
 *
 * @param arg0 0 draws the layout tables; any other value skips straight to
 *        the display requests.
 * @return Nothing.
 *
 * @par Confirmed
 * With arg0 == 0: 28 entries of D_8009C0E4 (id, x, y, align) are drawn via
 * func_800788F0(0, x, y, 16, id, align, 8, 2), then 5 entries of D_8009C18C
 * as atlas sprites func_80082AB0(id, x, y, -1, -1, 20, 128, 1, 8). Always:
 * func_80078B34(0x6AE, 0, 0) and func_80078B34(0x10CC, 1, 0) queue the ids
 * 0x6AE and 0x10CC into the two display slots of D_800A3578 (mode 0xA0).
 *
 * @par Likely role
 * The background pass of a menu screen: fixed labels and icons plus two
 * overlay requests (likely the screen's movie or fade ids).
 *
 * @par Unknown
 * What the display ids 0x6AE and 0x10CC select.
 *
 * @par Callers
 * No direct callers in the call graph. func_80077AD4 stores the address in
 * the screen callback block (D_800A3688.unk18 = D_8007E67C), so the screen
 * dispatcher reaches it indirectly.
 */
void gDrawFrontEndStatics(s32 arg0) __asm__("func_8007E67C");

void gDrawFrontEndStatics(s32 arg0)
{
    Entry *p;
    u32 i;

    if (arg0 == 0) {
        p = gMenuEntryTable;
        i = 0;
        while (i < 28) {
            func_800788F0(0, p->x, p->y, 16, p->id, p->align, 8, 2);
            i++;
            p++;
        }
        p = gMenuTailEntryTable;
        i = 0;
        while (i < 5) {
            func_80082AB0(p->id, p->x, p->y, -1, -1, 20, 128, 1, 8);
            i++;
            p++;
        }
    }
    func_80078B34(0x6AE, 0, 0);
    func_80078B34(0x10CC, 1, 0);
}
