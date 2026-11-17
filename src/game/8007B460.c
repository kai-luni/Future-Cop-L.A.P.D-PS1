// Categories: Game Engine > Front-End Configuration | Graphics > HUD > Slot List | Graphics > Sprites > Atlas
#include "game/types.h"

struct MenuEntry {
    u8 *text;
    s32 value;
    u8 pad08[4];
    s16 unk0C;
    u8 unk0E;
    u8 pad0F;
    u16 unk10;
    u16 unk12;
    u16 unk14;
    u16 unk16;
    u8 unk18;
    u8 pad19[3];
    u16 unk1C;
    u8 unk1E;
    u8 state;
    u8 pad20[4];
};

struct State8009FA94 {
    u8 pad00[0xC];
    s32 unk0C;
    u8 pad10[0x3C];
    s32 unk4C;
    u8 pad50[0xC];
    s32 unk5C;
};

extern u8 gSelectedRow __asm__("D_8009F87E");
extern u8 gLineCount __asm__("D_8009F87F");
extern u8 gSelectedMissionIndex __asm__("D_8009F880");
extern u8 D_8009F881;
extern u8 gMissionListMaxIndex __asm__("D_8009F882");
extern s8 *D_8009F884;
extern s8 D_8009F889;
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern s8 gUiHoldFlag __asm__("D_8009F8BD");
extern s8 gPlayerMenuDepth __asm__("D_8009F8C0");
extern  s8 gPlayerMenuDepth_arr[] __asm__("D_8009F8C0");
extern u8 *gPendingRowCapture __asm__("D_8009F8C4");
extern s32 gDeferredTransition __asm__("D_8009F8C8");
extern u8 *gPendingModuleNameFlag __asm__("D_8009F3EC");
extern u8 gModuleLoadState __asm__("D_8009F3F0");
extern s8 *gFontBankTable __asm__("D_8009F468");
extern u8 gHighlightShadeTable[] __asm__("D_8009F3F8");
extern s8 gCrimeWarProgress[] __asm__("D_8009F458");
extern u8 D_8009F444[];
extern u8 gMenuValueFormat[] __asm__("D_8009F484");
extern u8 D_8009F488[];
extern s32 gPaletteCycleCounter __asm__("D_8009FA60");
extern struct State8009FA94 *gGameSession __asm__("D_8009FA94");
extern struct MenuEntry gWideTextTable[] __asm__("D_8009B4E4");
extern u8 gFrontEndModuleName[] __asm__("D_8009B300");
extern u8 D_8009B3D4[];
extern s8 D_800A3628[];
extern s8 D_800A3658[];
extern s8 gScreenDescriptor[] __asm__("D_800A3838");
extern u8 D_80012088[];
extern u8 D_80012094[];

extern void func_80082AB0(s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_800788F0(u8 *, s32, s32, s32, s32, s32, s32, s32);
extern void func_80082B0C(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern s32 func_8008CD30(u8 *, u8 *, s32);
extern void func_8008CCB0(u8 *, u8 *);
extern s32 func_8008CCA0(u8 *, u8 *);
extern void func_80078650(s8 *, s32);
extern void func_80078B34(s32, s32, s32);
extern s32 func_80046C84(s32, s32);
extern void func_800493C0(u32, u8, u8, u8);

/**
 * @brief Draws the profile slot list: two header sprites and one row per
 *        visible slot with name, state markers and a progress bar.
 *
 * @param arg0 Must be 0 (the draw action); any other value returns.
 * @return Nothing.
 *
 * @par Confirmed
 * Atlas sprites 9 and 8 at (0x24, 0x34)/(0x93, 0x34), link 0x1F4. The row
 * window runs i = D_8009F881 .. D_8009F882 over the entries
 * D_8009B4E4[D_8009F884[i]]. The selected row (i == D_8009F880, edit flag
 * D_8009F8C0 set, mode D_8009F468[0] == 0) uses the cycling palette colour
 * D_8009F3F8[(D_8009FA60 >> 4) & 7] + 0x80 and draws the entry name text
 * (e->unk10) at (0x97, 0xB1); plain rows use 0x80. State markers: text
 * 0x5226 at (0x97, 0xA9) when D_8009FA94->unk5C == 1 and state != 0xA on
 * row 0, text 0xC8A when the row is below the D_8009F458[D_8009F889]
 * threshold or state == 0xA in the two-player mode (unk5C == 1, unk0C ==
 * 1, edit flag 2 forces colour 0xFF). In that mode a level number
 * m = clamp(state + 1, 1, 0xA) is formatted via D_8009F484 and drawn at
 * (e->unk1C, e->unk1E). The bar sprite uses len = e->unk12 below the
 * threshold (or at state 0xA), else e->unk14, drawn via func_80082B0C at
 * (e->unk16, e->unk18).
 *
 * @par Likely role
 * The profile/level select list of the front end: each row shows the
 * saved name, a completion marker and the progress bar; the two-player
 * mode adds the level number badge.
 *
 * @par Unknown
 * What state 0xA marks (likely "completed") and the threshold table's
 * meaning.
 *
 * @par Callers
 * func_8007B854, func_8007BCF4.
 */
void func_8007B460(s32 arg0)
{
    char text[8];
    s32 end;
    s32 i;
    struct MenuEntry *e;
    s32 c1, c2, c3;
    s32 len;
    s32 flag;
    s32 neg;

    if (arg0 != 0) {
        return;
    }

    neg = -1;
    func_80082AB0(9, 0x24, 0x34, neg, neg, 0x1F4, 0x80, 1, 8);
    func_80082AB0(8, 0x93, 0x34, neg, neg, 0x1F4, 0x80, 1, 8);

    end = (s8)gMissionListMaxIndex - (s8)D_8009F881 + 1;
    i = (s8)D_8009F881;
    if (i >= end) {
        return;
    }

    do {
        e = &gWideTextTable[D_8009F884[i]];

        if (gFontBankTable[arg0] != 0) {
            goto plain;
        }
        if (gPlayerMenuDepth == 0) {
            goto plain;
        }
        if (i != (s8)gSelectedMissionIndex) {
            goto plain;
        }

        flag = 0;
        __asm__("" : "+r"(flag));
        c1 = ({ register s32 idx __asm__("$2") = gPaletteCycleCounter; register s32 base; __asm__("lui\t%0,0x800a\n\taddiu\t%0,%0,-3080" : "=r"(base) : "r"(idx)); *(u8 *)(((idx >> 4) & 7) + base); }) + 0x80;
        c2 = c1;
        func_800788F0(0, 0x97, 0xB1, 0x10, e->unk10, 1, 8, 2);
        c3 = c1;

        if (i < ({ register s32 idx __asm__("$2") = D_8009F889; register s8 *base __asm__("$3"); __asm__("" : "+r"(idx)); base = gCrimeWarProgress; __asm__("" : "+r"(base)); *(s8 *)(idx + (s32)base); })) {
            flag = 1;
            goto common;
        }
        if (i != 0) {
            goto common;
        }
        if (gGameSession->unk5C == 1 && e->state != 0xA) {
            func_800788F0(0, 0x97, 0xA9, 0xC, 0x5226, 1, 8, 2);
        }

    common:
        if (gGameSession->unk5C == 1 && gGameSession->unk0C == 1) {
            if (gPlayerMenuDepth == 2) {
                c1 = 0xFF;
                c2 = c1;
                c3 = c1;
            }
            if (e->state == 0xA) {
                flag = 1;
            }
        }
        if ((flag & 0xFF) != 0) {
            func_800788F0(0, 0x97, 0xA9, 0x10, 0xC8A, 1, 8, 2);
        }
        goto cont;

    plain:
        c1 = 0x80;
        c2 = c1;
        c3 = c1;

    cont:
        if (i < ({ register s32 idx __asm__("$2") = D_8009F889; register s8 *base __asm__("$3"); __asm__("" : "+r"(idx)); base = gCrimeWarProgress; __asm__("" : "+r"(base)); *(s8 *)(idx + (s32)base); })) {
            len = e->unk12;
        } else {
            len = e->unk14;
        }

        if (gGameSession->unk5C == 1 && gGameSession->unk0C == 1) {
            s32 n = e->state + 1;

            s32 m;
            if (n >= 1) {
                m = 0xA;
                if (n < 0xB) {
                    m = n;
                }
            } else {
                m = 1;
            }
            { register u8 *src __asm__("$5") = gMenuValueFormat; __asm__("" : "+r"(src)); func_8008CD30(text, src, m); }
            func_800788F0(text, e->unk1C, e->unk1E, 0x10, 0, 4, 0x20, 0);
            if (e->state == 0xA) {
                len = e->unk12;
            }
        }

        func_80082B0C(len, e->unk16, e->unk18, -1, -1, 0xF, c3, c2, c1, 1, 8);
        i++;
    } while (i < end);
}
