// Categories: Game Engine > Front-End > List Screen | Graphics > HUD > Menu Text | Graphics > Text > Rendering
#include "game/types.h"

struct State8009FA94 {
    u8 pad00[0xC];
    s32 unk0C;
    u8 pad10[0x4C];
    s32 unk5C;
};

extern s8 *gFontBankTable __asm__("D_8009F468");
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern s8 gSelectedRow __asm__("D_8009F87E");
extern s8 gLineCount __asm__("D_8009F87F");
extern s8 gPlayerMenuDepth __asm__("D_8009F8C0");
extern  s8 gPlayerMenuDepth_arr[] __asm__("D_8009F8C0");
extern s32 gDeferredTransition __asm__("D_8009F8C8");
extern s32 gPaletteCycleCounter __asm__("D_8009FA60");
extern struct State8009FA94 *gGameSession __asm__("D_8009FA94");
extern u8 gMenuValueFormat[] __asm__("D_8009F484");
extern u8 D_8009B3D4[];
extern s8 D_800A3838[];

extern void func_8007B460(s32);
extern void func_800788F0(u8 *, s32, s32, s32, s32, s32, s32, s32);
extern void func_800493C0(u32, u8, u8, u8);
extern s32 func_8008CD30(u8 *, u8 *, s32);
extern void func_80078B34(s32, s32, s32);

/**
 * @brief Draws one front-end list screen: the profile list body, a numbered row
 *        list with one highlighted line, and two per-screen display ids.
 *
 * @param arg0 Draw selector; any non-zero value returns at once, so only a 0
 *        performs the pass.
 * @return Nothing.
 *
 * @par Confirmed
 * When D_8009F468 differs from D_800A3838 the profile list is redrawn with
 * func_8007B460(D_8009F8B8). The row type is D_8009F8C0_arr[D_8009F8B8]; when
 * it is 2 a header with text id 0xC80 is drawn centred at (0x57, 0x3B) through
 * func_800788F0, then rows 1 to D_8009F87F + 1 are formatted with
 * func_8008CD30(text, D_8009F484, i) and drawn at (0x57, y) with y from 0x52
 * stepping by 0xA. The row equal to D_8009F87E gets colour 0x11 and also
 * recolours palette entry 0x11 via func_800493C0(0x11, ...) from
 * D_8009B3D4[((D_8009FA60 >> 4) & 7) * 3]; every other row uses 0xF. When
 * D_8009F8C8 is not -1 the function returns before the display ids. With the row
 * type 2 the ids are a0 = 0xC6C and s0 = 0xC62; otherwise the first byte of the
 * pushed screen block, *D_8009F468, selects a pair through a six-entry jump
 * table: 0 gives 0xC26/0xC1C with the edit flag D_8009F8C0 set and 0xC4E
 * otherwise, 1 gives 0xC44, 2 gives 0xC56, 3 gives 0xC3A but 0xC30 when
 * D_8009FA94 +0x5C is 1 and 0xC3A again when its +0x0C is 1, 4 gives 0xC53 and
 * 5 gives 0xC30; ids above 5 fall through with a0 = 0 and s0 = 0x44C. In every
 * case bit 9 of D_8009FA60 forces s0 to 0x10CC, then func_80078B34(a0, 0, 0)
 * and func_80078B34(s0, 1, 0) queue the two ids into the two display slots.
 *
 * @par Likely role
 * The common draw and update callback of the three front-end list screens: the
 * level or profile list on the left, a numbered sub-list with the selected line
 * in the cycling highlight colour, and one display id per screen - the ids in
 * the 0xC20..0xC6C range differ per screen and per two-player state, which reads
 * like a page label or prompt pair.
 *
 * @par Unknown
 * What the display ids actually select, why the two-player words at
 * D_8009FA94 +0x5C/+0x0C only affect screen 3, and what the pending value
 * D_8009F8C8 suppresses the ids for.
 *
 * @par Callers
 * No direct callers in the call graph. func_80077AD4 stores the address in the
 * +0x18 callback field of the three screen blocks D_800A3868, D_800A3898 and
 * D_800A38C8, which also share the +0x10 pointer D_8007B3F4 and the +0x20
 * pointer D_8007F618, so the screen dispatcher reaches it indirectly.
 */
void func_8007BCF4(s32 arg0)
{
    u8 text[8];
    s32 y;
    s32 i;
    s32 c;
    s32 a0;
    s32 s0;
    s32 v;
    s32 k;
    static void *const label_table[] __attribute__((used)) = {
        &&case_0, &&case_1, &&case_2, &&case_3, &&case_4, &&case_5
    };

    if (arg0 != 0) {
        return;
    }

    if (gFontBankTable != D_800A3838) {
        func_8007B460(gActivePlayerIndex);
    }

    { register s32 idx __asm__("$2") = gActivePlayerIndex; register s8 *base __asm__("$3"); __asm__("" : "+r"(idx)); base = gPlayerMenuDepth_arr; __asm__("" : "+r"(base)); v = *(s8 *)(idx + (s32)base); }
    if (v == 2) {
        y = 0x52;
        func_800788F0(0, 0x57, 0x3B, 0x10, 0xC80, 4, 8, v);
        i = 0;
        if (gLineCount + 1 > 0) {
            do {
                if (i == gSelectedRow) {
                    u8 *p;
                    c = 0x11;
                    p = D_8009B3D4 + ((gPaletteCycleCounter >> 4) & 7) * 3;
                    func_800493C0(c, p[0], p[1], p[2]);
                } else {
                    c = 0xF;
                }
                { register u8 *t __asm__("$4") = text; register u8 *src __asm__("$5") = gMenuValueFormat; __asm__("" : "+r"(t)); __asm__("" : "+r"(src)); func_8008CD30(t, src, ++i); }
                func_800788F0(text, 0x57, y, c, 0, 4, 8, 2);
                y += 0xA;
            } while (i < gLineCount + 1);
        }
    }

    if (gDeferredTransition != -1) {
        return;
    }

    { register s32 idx __asm__("$2") = gActivePlayerIndex; register s8 *base __asm__("$3"); __asm__("" : "+r"(idx)); base = gPlayerMenuDepth_arr; __asm__("" : "+r"(base)); v = *(s8 *)(idx + (s32)base); }
    if (v == 2) {
        a0 = 0xC6C;
        s0 = 0xC62;
        goto common;
    }
    a0 = 0;
    s0 = 0x44C;
    k = *gFontBankTable;
    if ((u32)k >= 6) {
        goto common;
    }
    goto *label_table[k];

case_0:
    if (gPlayerMenuDepth != 0) {
        a0 = 0xC26;
        s0 = 0xC1C;
    } else {
        a0 = 0xC4E;
    }
    goto common;
case_1:
    a0 = 0xC44;
    goto common;
case_2:
    a0 = 0xC56;
    goto common;
case_3:
    a0 = 0xC3A;
    if (gGameSession->unk5C == 1) {
        a0 = 0xC30;
        if (gGameSession->unk0C == 1) {
            a0 = 0xC3A;
        }
    }
    goto common;
case_4:
    a0 = 0xC53;
    goto common;
case_5:
    a0 = 0xC30;

common:
    if ((gPaletteCycleCounter >> 9) & 1) {
        s0 = 0x10CC;
    }
    func_80078B34(a0, 0, 0);
    func_80078B34(s0, 1, 0);
}
