// Categories: Game Engine > Front-End > Character Entry | Graphics > Text > Rendering | Graphics > 2D Drawing | Platform > Sound
#include "game/types.h"

struct FontEntry {
    u8 unk00[6];
    s16 height;
    s16 unk8;
};

extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern s32 gScalarCursor __asm__("D_8009F8F8");
extern  s32 gScalarCursor_arr[] __asm__("D_8009F8F8");
extern u16 gStatusTextId __asm__("D_8009F92A");
extern u8 gPasswordBuffer[] __asm__("D_800A3948");
extern u8 gPackedPasswordBytes[] __asm__("D_800A3938");
extern u8 gHexDigitTable[] __asm__("D_8009C1AC");
extern u8 gOctalDigitTable[] __asm__("D_8009C1BC");
extern u8 gHighlightShadeTable[8] __asm__("D_8009F3F8");
extern s32 gGameClock[] __asm__("D_8009FA60");
extern void *jtbl_800121A4[];

extern s32 func_8007EA78(void);
extern s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1) __asm__("func_80046C84");
extern void gPopMenuPanel(void) __asm__("func_80078740");
extern void gSetTextPaletteEntry(u32 index, u8 blue, u8 green, u8 red) __asm__("func_800493C0");
extern void gDrawAlignedMultilineText(u8 *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7) __asm__("func_800788F0");
extern void func_80078B34(s32 arg0, s32 arg1, s32 arg2);
extern void gEmitFlatQuadRect(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7) __asm__("func_80082EF0");
extern void func_8007F128(s32 input0, s32 input1, s32 input2, s32 input3, s32 arg5, s32 arg6);
extern void gDrawAlignedString(u8 *text, s32 x, s32 y, s32 color) __asm__("func_80049100");
extern void gMeasureTextExtent(u8 *text, s32 *width, s32 *height) __asm__("func_800492A0");
extern struct FontEntry *gGetActiveFont(void) __asm__("func_80048E88");
void gUpdatePasswordScreen(s32 state, s32 input_baseX) __asm__("func_8007F1F8");


/**
 * @brief State machine and renderer of the ten-character entry screen: commit,
 *        step one letter, move the cursor, or draw the whole panel.
 *
 * @param state Action selector 0..5; values of 6 and above return immediately.
 * @param input_baseX Horizontal centre of the ten-letter row.
 * @return Nothing.
 *
 * @par Confirmed layout
 * D_800A3948 holds ten letter indices in 0..25, D_8009F8F8 is the edit cursor in
 * 0..9, D_8009F8B8 selects the row of the word array D_8009F8F8_arr whose entry
 * marks the highlighted column, D_8009F92A is a status text id (0 while no
 * message is pending), D_8009FA60 is a frame counter and D_8009FA60[0] supplies
 * the palette and toggle bits. The dispatch is a jump through the table
 * jtbl_800121A4, and the local array of label addresses is emitted as well.
 *
 * @par Confirmed states
 * 0 - commit: func_8007EA78 packs the ten letters; a zero low byte means the
 * string is not acceptable, so the status text is set to 0x3AAC and sound 0x4C
 * plays, otherwise sound 0x4D plays and func_80078740 pops the front-end panel.
 * 1 and 2 - step the current letter down or up with wrap 0..0x19, sound 0x46.
 * 3 and 4 - move the cursor with wrap 0..9, sound 0x47.
 * Every one of these first clears D_8009F92A.
 * 5 - draw: when a status text is pending it is coloured with
 * func_800493C0(0x11, 0x1F, 0x0A, 0x0A) and drawn at (0xA0, 0x82). Two numeric
 * fields are drawn with func_80078B34, the ids switching on bit 9 of D_8009FA60.
 * Two more text blocks (0x3A98 at y 0x3C and 0x3AB6 at y 0xA2) and the frame
 * func_8007F128(0xA0, 0xAD, 0x0F, 2, 4, 8) follow. The letter row is built by
 * adding 0x41 to each stored index, measured with func_800492A0 and drawn by
 * func_80049100 at y 100, advancing by the font width + 1 from
 * baseX - 5 * (width + 1); the highlighted column also gets a gouraud quad from
 * func_80082EF0 in colour D_8009F3F8[(D_8009FA60[0] >> 4) & 7] + 0x80 and is
 * drawn with palette index 8 instead of 15.
 *
 * @par Likely
 * A password or initials panel: ten letters A..Z, cursor left/right, letter
 * up/down, and a commit that validates the string. func_8007EA78 encodes the ten
 * characters through a 16-symbol and an 8-symbol alphabet, which fits a code
 * rather than a name.
 *
 * @par Unknown
 * The relation between the scalar cursor D_8009F8F8 written by states 1..4 and
 * the array entry D_8009F8F8_arr[D_8009F8B8] used for the highlight is not
 * resolvable from this file, and the numeric fields drawn in state 5 are not
 * identified by their ids.
 *
 * @par Callers
 * None in analysis/edges.json, not even an address-taken edge, so the caller lies
 * outside the analysed set; func_8007F1F8 is reached indirectly.
 *
 * @par Environment path
 * Jump-table state machine (states 0..5 via jtbl_800121A4) for the
 * 10-character A..Z password entry panel. State 0 validates gPasswordBuffer
 * via func_8007EA78, states 1..4 edit letters and gScalarCursor, and state 5
 * draws the panel, prompts via func_80078B34, and the highlighted 10-letter
 * row via func_80082EF0 and func_80049100.
 */
void gUpdatePasswordScreen(s32 state, s32 input_baseX)
{
    u8 text[8];
    s32 width;
    s32 height;
    s32 i;
    s32 x;
    s32 y;
    s32 color;
    register s32 boxX __asm__("$4");
    register s32 boxY __asm__("$5");
    register s32 boxW __asm__("$6");
    register s32 boxKind __asm__("$7");
    register s32 *sel __asm__("$20");
    register s32 *base __asm__("$4");
    register s32 off __asm__("$3");
    s32 fontUnk8;
    register s32 fontHeight __asm__("$21");
    s32 hundred;
    u8 *chars;
    struct FontEntry *font;
    s32 idx;
    s32 baseX;

    static void *const label_table[] __attribute__((used)) = {
        &&state_0, &&state_1, &&state_2, &&state_3, &&state_4, &&state_5
    };

    baseX = input_baseX;
    idx = gActivePlayerIndex;
    if ((u32)state >= 6) {
        goto done;
    }
    goto *jtbl_800121A4[(u32)state];
state_0:
        if ((func_8007EA78() & 0xFF) == 0) {
            gStatusTextId = 0x3AAC;
            gPlayNonpositionalSoundEffect(0x4C, 0);
        } else {
            gPlayNonpositionalSoundEffect(0x4D, 0);
            gPopMenuPanel();
        }
        goto done;
state_1:
        gStatusTextId = 0;
        gPlayNonpositionalSoundEffect(0x46, 0);
        {
            u8 *p;
            i = gScalarCursor;
            p = gPasswordBuffer + i;
            if (*p == 0) {
                *p = 0x19;
            } else {
                (*p)--;
            }
        }
        goto done;
state_2:
        gStatusTextId = 0;
        gPlayNonpositionalSoundEffect(0x46, 0);
        {
            u8 *p;
            i = gScalarCursor;
            p = gPasswordBuffer + i;
            (*p)++;
            if (*p >= 0x1A) {
                *p = 0;
            }
        }
        goto done;
state_3:
        gStatusTextId = 0;
        gScalarCursor++;
        if (gScalarCursor >= 0xA) {
            gScalarCursor = 0;
        }
        gPlayNonpositionalSoundEffect(0x47, 0);
        goto done;
state_4:
        gStatusTextId = 0;
        gScalarCursor--;
        if (gScalarCursor < 0) {
            gScalarCursor = 9;
        }
        gPlayNonpositionalSoundEffect(0x47, 0);
        goto done;
state_5:
        if (gStatusTextId != 0) {
            gSetTextPaletteEntry(0x11, 0x1F, 0xA, 0xA);
            gDrawAlignedMultilineText(0, 0xA0, 0x82, 0x11, gStatusTextId, 4, 8, 1);
        }
        if ((gGameClock[0] >> 9) & 1) {
            func_80078B34(0x6E5, 0, 0);
            func_80078B34(0x6E0, 1, 0);
        } else {
            func_80078B34(0x3AA2, 0, 0);
            func_80078B34(0x10CC, 1, 0);
        }
        gDrawAlignedMultilineText(0, 0xA0, 0x3C, 0x10, 0x3A98, 4, 8, 1);
        gDrawAlignedMultilineText(0, 0xA0, 0xA2, 0x10, 0x3AB6, 4, 8, 2);
        func_8007F128(0xA0, 0xAD, 0xF, 2, 4, 8);

        hundred = 100;
        font = gGetActiveFont();
        fontUnk8 = font->unk8;
        font = gGetActiveFont();
        fontHeight = font->height;
        chars = gPasswordBuffer;
        x = baseX - 5 * (fontUnk8 + 1);
        text[1] = 0;
        {
            base = gScalarCursor_arr;
            off = idx << 2;
            __asm__("" : "+r"(off) : "r"(fontHeight));
            sel = (s32 *)((u8 *)base + off);
            for (i = 0; i < 0xA; i++) {
                text[0] = *(u8 *)((u32)i + (u32)chars) + 0x41;
                gMeasureTextExtent(text, &width, &height);
                if (i == *sel) {
                    boxX = x;
                    boxY = hundred + fontHeight + 2;
                    boxW = fontUnk8;
                    boxKind = 2;
                    __asm__ volatile("" : "+r"(boxKind) : "r"(boxX), "r"(boxY), "r"(boxW));
                    color = gHighlightShadeTable[(gGameClock[0] >> 4) & 7] + 0x80;
                    gEmitFlatQuadRect(boxX, boxY, boxW, boxKind, 10, color, color, color);
                    y = 8;
                } else {
                    y = 0xF;
                }
                gDrawAlignedString(text, x + (fontUnk8 - width) / 2, hundred, y);
                { s32 t = x + 1; x = t + fontUnk8; }
            }
        }
        goto done;
done:
    ;
}
