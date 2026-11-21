// Categories: Game Engine > Front-End Configuration | Graphics > HUD > Name Entry | Graphics > 2D Drawing > Text | Resources > Slot Table
#include "game/types.h"

typedef struct {
    u8 pad0[0x1F];
    u8 unk1F;
    u8 unk20;
    u8 pad21[3];
} Entry;

extern u8 gUiHoldFlag __asm__("D_8009F8BD");
extern u8 gUiInteractionEnabled __asm__("D_8009F8BE");
typedef struct { u8 b[0x70]; } Rec;
typedef struct {
    u8 pad0[0x424]; Rec recs[10]; u8 pad884[0x9D];
    u8 mode; u8 pad922[2]; u8 index; u8 name[0x1D]; u16 timer;
} Big;
extern Big *gSaveProfileImage __asm__("D_8009F92C");
#define RAW ((u8 *)gSaveProfileImage)
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern u8 *gNameEntryAlphabet __asm__("D_8009F878");
extern s16 gPlayerNameLength __asm__("D_8009F87C");
extern s8 gMissionListMaxIndex __asm__("D_8009F882");
extern s8 gSelectedMissionIndex __asm__("D_8009F880");
extern s8 gMissionListMinIndex __asm__("D_8009F881");
extern s8 gPlayerMenuDepth[] __asm__("D_8009F8C0");
extern u8 gNameEntryGlyphBuf[] __asm__("D_800A3928");
extern s32 gScalarCursor[] __asm__("D_8009F8F8");
extern u8 gPlayerNameTable[] __asm__("D_800A6FE0");
extern u8 *gGameSession __asm__("D_8009FA94");
extern u8 gCrimeWarProgress __asm__("D_8009F458");
extern u8 gPrecinctAssaultProgress __asm__("D_8009F459");
extern u8 gSavedCrimeWarProgress __asm__("D_8009F3F1");
extern u8 gFrontEndListFlag __asm__("D_8009F3C0");
extern u8 gPlayerWrapLimit __asm__("D_8009F3C1");
extern u8 gPausedMenuFlag __asm__("D_8009F3C3");
extern u8 gLockedMenuFlag __asm__("D_8009F3C4");
extern u8 gLayoutPointerFlagA __asm__("D_8009F3E6");
extern u8 gLayoutPointerFlagB __asm__("D_8009F3E7");
extern u8 gRowClearFlag __asm__("D_8009FA10");
extern u8 *gFontBankTable __asm__("D_8009F468");
extern Entry gWideTextTable[] __asm__("D_8009B4E4");
extern s8 gFrontEndListTable[] __asm__("D_8009B72C");
extern u8 gSkyProfileLabelFormat[] __asm__("D_800120BC");
extern u8 gSkyProfileFallbackLabelFormat[] __asm__("D_800120C8");
extern u8 gZoneProfileFallbackLabelFormat[] __asm__("D_800120D4");
extern u8 gMenuValueFormat[] __asm__("D_8009F484");
extern u16 gPlaceholderGlyph __asm__("D_8009F470");
extern u8 gCrimeWarProfileNameFormat[] __asm__("D_8009F498");
extern u8 gProfileNameCellWidthText[] __asm__("D_8009F4A0");
extern u8 gDefaultNameString[] __asm__("D_8009F4A4");
extern u8 gMenuActivationBlock[] __asm__("D_800A3718");
extern u8 gHighlightShadeTable[] __asm__("D_8009F3F8");
extern s32 gGameClock __asm__("D_8009FA60");

extern void func_8007AA9C(u8 *a, u8 *b, s32 c, u8 *d);
extern void func_8007A9D0(u8 *a, u8 *b, s32 c, u8 *d, s32 e);
extern void gSaveSelectedProfileToCard(void) __asm__("func_80080B70");
extern void func_80080984(u8 *a, s32 b);
extern void func_8008CCB0(u8 *a, u8 *b);
extern s32 func_8008CCC0(u8 *a);
extern s32 gSpillVarargsArguments(u8 *a, u8 *fmt, ...) __asm__("func_8008CD30");
extern u8 *gLookupTextString(s32 id) __asm__("func_80089914");
extern void gActivateMenuPanel(u8 *a, s32 b) __asm__("func_80078650");
extern void gPlayNonpositionalSoundEffect(s32 id, s32 b) __asm__("func_80046C84");
extern void gMeasureTextExtent(u8 *text, s32 *w, s32 *h) __asm__("func_800492A0");
extern s32 *gGetActiveFont(void) __asm__("func_80048E88");
extern void gEmitFlatQuadRect(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h) __asm__("func_80082EF0");
extern void gDrawAlignedString(u8 *text, s32 x, s32 y, s32 color) __asm__("func_80049100");
extern void gDrawAlignedMultilineText(u8 *a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h) __asm__("func_800788F0");
extern void func_80077FFC(void);
extern void func_80077708(void);
extern void func_800776B8(void);
extern void gReleaseProfileCardOperation(void) __asm__("func_8007FACC");
extern void gRouteFrontEndModeMenu(void) __asm__("func_80078210");

#define REC(off) (gSaveProfileImage->recs[gSaveProfileImage->index].b[(off) - 0x424])
#define RECP(base, off) ((base) + (base)[0x924] * 0x70 + (off))
#define CNT(e) \
    if ((e).unk1F + 1 > 0) { \
        if ((e).unk1F + 1 < 11) { n += 1 + (e).unk1F; } \
        else { n += 10; } \
    } else { n += 1; }
void gHandleProfileSlotWidget(u32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) __asm__("func_8008125C");


/**
 * @brief Front-end slot widget: dispatches one action code onto the profile-slot
 *        page, edits the slot's stored name and, on confirm, either applies the
 *        slot's saved configuration to the running game or advances the page.
 *
 * @param arg0 Action code 0..5: 0 draw the row, 1/2 move the glyph cursor, 3 confirm
 *        the slot, 4/5 step the glyph under the cursor. Values above 5 do nothing.
 * @param arg1 Horizontal centre of the eight-cell glyph strip; the strip is laid out
 *        from arg1 - 4 * (advance + 1) and the text is drawn around it.
 * @param arg2 y position of the row.
 * @param arg3 Text colour, used only by the non-editing draw path.
 * @param arg4 Slot number; stored as the slot block's selected index on confirm.
 * @return Nothing.
 *
 * @par Confirmed gate
 * The body runs only when D_8009F8BD and D_8009F8BE are both non-zero - the two
 * display flags func_8007FACC raises - and arg0 is below 6. s2 is the local player
 * index D_8009F8B8, indexing the edit flags D_8009F8C0[], the cursor words
 * D_8009F8F8[], the eight-byte glyph buffers at D_800A3928 and the nine-byte name
 * buffers at D_800A6FE0 (eight characters plus terminator).
 *
 * @par Confirmed slot block
 * D_8009F92C points at a block with ten records of 0x70 bytes from +0x424, a page byte
 * at +0x921, the selected record index at +0x924, a 0x1D-byte scratch name at +0x925,
 * a bulk-upload half-word at +0x942 and screen state bytes at +0x200 to +0x204.
 * Record-relative offsets used here are +0x428 (occupancy byte), +0x440 and +0x44C
 * (the two player names), +0x458 to +0x463, +0x464 to +0x46E (five per-entry counters)
 * and +0x474 (an audio options block).
 *
 * @par Confirmed actions
 * 0: while editing, each of the eight cells is drawn as
 * D_8009F878[D_800A3928[s2 * 8 + i]] with its width from func_800492A0 and spaced by
 * the font advance; the selected cell gets a gouraud box (func_80082EF0, link 0xBB8)
 * at y + font height + 2 in D_8009F3F8[(D_8009FA60 >> 4) and 7] + 0x80, glyph colour
 * 8 for the selection and 15 for the rest. Otherwise func_800788F0 draws one string
 * in colour arg3: the record name, else func_80089914(0x3714) or D_8009F4A4.
 * 1/2: cursor in 0..7 with wraparound, plus sound func_80046C84(0x47, 0).
 * 4/5: glyph index in 0..D_8009F87C - 1 with wraparound, plus sound
 * func_80046C84(0x46, 0). Both only react when D_8009F8C0[s2] is set and arg4 equals
 * the selected slot.
 *
 * @par Confirmed confirm path
 * The selected index and the timer of the slot block are set from arg4 and 0, then two
 * branches key on the page byte at +0x921. With it equal to 1 an active edit session is
 * decoded back into the record by func_8007AA9C and func_80080B70 refreshes the derived
 * state; otherwise a label is composed through func_8008CD30 by counting the words at
 * D_8009FA94 + 0x8C, +0x90 and +0x94 - or the placeholder half-word D_8009F470 when
 * all three are zero - and written to the record, after which func_8007A9D0 re-encodes
 * it into glyph indices. A non-empty record raises D_8009F468[0] to 1 and, after
 * func_80078650(D_800A3718, 1), writes 5, 0 and 0 into +0x200 to +0x204.
 * For any other page byte the slot is applied instead: func_80080984 loads the audio
 * block at +0x474, the names at +0x440 and +0x44C are copied into D_800A6FE0, and the
 * bytes at +0x459 to +0x463 are stored into the state words D_8009FA94 + 0x8C, +0x90,
 * +0x94, +0x28, +0x34, +0x2C, +0x38, +0x30, +0x3C, +0x08, +0x98, +0x0C and +0x5C.
 * Then +0x46A and +0x46B go to D_8009F3E6 and D_8009F3E7, +0x469, +0x46C, +0x46D and
 * +0x46F conditionally to D_8009F3C0, D_8009F3C1, D_8009F3C3 and D_8009F3C4, and the
 * counters +0x464 to +0x467 and +0x46E to the entries addressed through D_8009B72C.
 * func_80077FFC follows, then either func_80077708 or func_800776B8 together with the
 * clamped value of +0x458 spread over D_8009F458, D_8009F3F1, D_8009F882, D_8009F881
 * and D_8009F880 (clamped to 0..7). D_8009FA10 is cleared, func_8007FACC commits a
 * pending movie start, and func_80078210 closes the transition.
 *
 * @par Likely role
 * The handler behind the ten rows of the front-end profile list: the page byte decides
 * between the naming page (finish the name, continue) and the slot page (apply the
 * profile and start the mission, intro movie included).
 *
 * @par Unknown
 * The meaning of the individual setting bytes +0x458 to +0x46F, whether the page byte
 * value 1 really is the naming page, and the text behind D_8009F470.
 *
 * @par Callers
 * No direct caller. Its address is the callback word at +4 of ten 0x1C-byte records
 * starting at 0x8009B8D0, all of which reference the text D_8009F470 - so it is reached
 * through the front-end widget list.
 *
 * @par Environment path
 * Front-end profile-slot widget handler gated by D_8009F8BD and D_8009F8BE.
 * Action 0 draws the 8-cell glyph editor with a Gouraud selection box via
 * func_80082EF0 (link 0xBB8) and func_80049100, or the slot label via
 * func_800788F0; actions 1/2/4/5 step cursor and glyph indices; action 3
 * commits the name or applies the saved profile to D_8009FA94 and transitions
 * via func_8007FACC and func_80078210.
 */
void gHandleProfileSlotWidget(u32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 s2;
    s32 n;
    s32 i;
    u8 *p;
    u8 *q;
    u8 *name;
    u8 *s;
    register s32 lenv __asm__("$3");
    register s32 step __asm__("$2");
    s32 y;
    s32 w, h;
    s32 w2, h2;
    u8 txt[8];
    u8 *base;
    Big *init;
    register s32 heightTmp __asm__("$5");
    register s32 one __asm__("$7");
    s32 *cursor;
    register u8 *textBase __asm__("$16");
    register u8 *savedBase __asm__("$3");
    register s32 savedIdx __asm__("$2");
    register s32 off __asm__("$5");
    register s32 selected __asm__("$3");
    register u8 *dst __asm__("$4");

    __asm__("" : : "r"(arg4) : "$18");
    s2 = gActivePlayerIndex;
    if (gUiHoldFlag != 0 && gUiInteractionEnabled != 0 && arg0 < 6) {
        switch (arg0) {
        case 3:
            init = gSaveProfileImage;
            init->index = arg4;
            init->timer = 0;
            base = RAW;
            if (base[0x921] == 1) {
                if (gPlayerMenuDepth[s2] != 0) {
                    gPlayerMenuDepth[s2] = 0;
                    func_8007AA9C(RAW + 0x925, gNameEntryGlyphBuf + s2 * 8, 8, gNameEntryAlphabet);
                    gSaveSelectedProfileToCard();
                    break;
                }
                selected = base[0x924];
                __asm__("" : "+r"(selected));
                step = (selected << 3) - selected;
                __asm__("" : "+r"(step));
                off = step << 4;
                if ((base + off)[0x428] == 0) {
                    s32 *fa = (s32 *)gGameSession;
                    s32 count;
                    count = fa[0x8C / 4] != 0;
                    if (fa[0x90 / 4] != 0) count++;
                    if (fa[0x94 / 4] != 0) count++;
                    if (count != 0) {
                        gSpillVarargsArguments(txt, gMenuValueFormat, count);
                    } else {
                        *(u16 *)txt = gPlaceholderGlyph;
                    }
                    if (*(s32 *)(gGameSession + 0x5C) == 1) {
                        n = 0;
                        for (arg2 = 0; arg2 < 4; arg2++) {
                            CNT(gWideTextTable[gFrontEndListTable[arg2]])
                        }
                        CNT(gWideTextTable[gFrontEndListTable[4]])
                        arg2 = (s32)gLookupTextString(0x52D0);
                        if (((u8 *)arg2) != 0 && func_8008CCC0(((u8 *)arg2)) < 5) {
                            gSpillVarargsArguments(RAW + 0x925, gSkyProfileLabelFormat, ((u8 *)arg2), n, txt);
                        } else {
                            gSpillVarargsArguments(RAW + 0x925, gSkyProfileFallbackLabelFormat, n, txt);
                        }
                    } else {
                        arg2 = (s32)gLookupTextString(0x3868);
                        if (((u8 *)arg2) != 0 && func_8008CCC0(((u8 *)arg2)) < 7) {
                            gSpillVarargsArguments(RAW + 0x925, gCrimeWarProfileNameFormat, ((u8 *)arg2), gMissionListMaxIndex + 1, txt);
                        } else {
                            gSpillVarargsArguments(RAW + 0x925, gZoneProfileFallbackLabelFormat, gMissionListMaxIndex + 1, txt);
                        }
                    }
                } else {
                    dst = base + 0x925;
                    __asm__("addu %0,%0,%1" : "+r"(off) : "r"(base), "r"(dst));
                    func_8008CCB0(dst, (u8 *)off + 0x428);
                }
                gScalarCursor[s2] = 0;
                func_8007A9D0(RAW + 0x925, gNameEntryGlyphBuf + s2 * 8, 8, gNameEntryAlphabet, gPlayerNameLength);
                if (REC(0x428) != 0) {
                    RAW[0x200] = 5;
                    RAW[0x201] = 0;
                    RAW[0x204] = 0;
                    gActivateMenuPanel(gMenuActivationBlock, 1);
                    *gFontBankTable = 1;
                } else {
                    gPlayerMenuDepth[s2] = 1;
                }
                break;
            }
            if (RAW[arg4 * 0x70 + 0x428] == 0) {
                break;
            }
            func_80080984(RECP(RAW, 0x474), 0);
            textBase = gPlayerNameTable;
            dst = textBase;
            savedBase = RAW;
            savedIdx = savedBase[0x924];
            off = savedIdx * 0x70;
            __asm__("addu %0,%0,%1" : "+r"(off) : "r"(savedBase), "r"(dst));
            func_8008CCB0(dst, (u8 *)off + 0x440);
            dst = textBase + 9;
            savedBase = RAW;
            savedIdx = savedBase[0x924];
            off = savedIdx * 0x70;
            __asm__("addu %0,%0,%1" : "+r"(off) : "r"(savedBase), "r"(dst));
            func_8008CCB0(dst, (u8 *)off + 0x44C);
            if (REC(0x459) != 0) *(s32 *)(gGameSession + 0x8C) = REC(0x459);
            if (REC(0x45A) != 0) *(s32 *)(gGameSession + 0x90) = REC(0x45A);
            if (REC(0x45B) != 0) *(s32 *)(gGameSession + 0x94) = REC(0x45B);
            *(s32 *)(gGameSession + 0x28) = REC(0x45C);
            *(s32 *)(gGameSession + 0x34) = REC(0x45D);
            *(s32 *)(gGameSession + 0x2C) = REC(0x45E);
            *(s32 *)(gGameSession + 0x38) = REC(0x45F);
            *(s32 *)(gGameSession + 0x30) = REC(0x460);
            *(s32 *)(gGameSession + 0x3C) = REC(0x461);
            *(s32 *)(gGameSession + 0x8) = REC(0x462);
            *(s32 *)(gGameSession + 0x98) = REC(0x463);
            *(s32 *)(gGameSession + 0xC) = REC(0x470);
            *(s32 *)(gGameSession + 0x5C) = REC(0x468);
            gLayoutPointerFlagA = REC(0x46A);
            gLayoutPointerFlagB = REC(0x46B);
            if (REC(0x469) != 0) gFrontEndListFlag = REC(0x469);
            if (REC(0x46C) != 0) gPlayerWrapLimit = REC(0x46C);
            if (REC(0x46D) != 0) gPausedMenuFlag = REC(0x46D);
            if (REC(0x46F) != 0) gLockedMenuFlag = REC(0x46F);
            for (arg2 = 0; arg2 < 4; arg2++) {
                gWideTextTable[gFrontEndListTable[arg2]].unk1F = REC(0x464 + arg2);
                gWideTextTable[gFrontEndListTable[arg2]].unk20 = REC(0x464 + arg2);
            }
            gWideTextTable[gFrontEndListTable[4]].unk1F = REC(0x46E);
            gWideTextTable[gFrontEndListTable[4]].unk20 = REC(0x46E);
            func_80077FFC();
            if (*(s32 *)(gGameSession + 0x5C) == 1) {
                gPrecinctAssaultProgress = 0;
                func_80077708();
            } else {
                s32 v;
                s32 saved;
                func_800776B8();
                saved = REC(0x458);
                gCrimeWarProgress = saved;
                gSavedCrimeWarProgress = REC(0x458);
                if ((s8)saved >= 0) {
                    if ((s8)saved >= 8) v = 7;
                    else v = saved;
                } else {
                    v = 0;
                }
                gMissionListMaxIndex = v;
                gMissionListMinIndex = 0;
                gSelectedMissionIndex = v;
            }
            gRowClearFlag = 0;
            gReleaseProfileCardOperation();
            gRouteFrontEndModeMenu();
            break;
        case 4:
            *(u16 *)(RAW + 0x942) = 0;
            if (gPlayerMenuDepth[s2] != 0 && arg4 == RAW[0x924]) {
                gPlayNonpositionalSoundEffect(0x46, 0);
                arg2 = gScalarCursor[s2];
                arg3 = (s32)(gNameEntryGlyphBuf + s2 * 8);
                p = (u8 *)arg3 + arg2;
                if (*p != 0) {
                    *p = *p - 1;
                } else {
                    *p = *(u8 *)&gPlayerNameLength - 1;
                }
            }
            break;
        case 5:
            *(u16 *)(RAW + 0x942) = 0;
            if (gPlayerMenuDepth[s2] != 0 && arg4 == RAW[0x924]) {
                gPlayNonpositionalSoundEffect(0x46, 0);
                arg2 = gScalarCursor[s2];
                arg3 = (s32)(gNameEntryGlyphBuf + s2 * 8);
                p = (u8 *)arg3 + arg2;
                *p = *p + 1;
                if (*p >= gPlayerNameLength) {
                    *p = 0;
                }
            }
            break;
        case 1:
            if (gPlayerMenuDepth[s2] != 0 && arg4 == RAW[0x924]) {
                step = s2 << 2;
                __asm__("" : "+r"(step));
                cursor = (s32 *)(step + (u8 *)gScalarCursor);
                *cursor = *cursor + 1;
                if (*cursor >= 8) {
                    *cursor = 0;
                }
                gPlayNonpositionalSoundEffect(0x47, 0);
            }
            break;
        case 2:
            if (gPlayerMenuDepth[s2] != 0 && arg4 == RAW[0x924]) {
                step = s2 << 2;
                __asm__("" : "+r"(step));
                cursor = (s32 *)(step + (u8 *)gScalarCursor);
                *cursor = *cursor - 1;
                if (*cursor < 0) {
                    *cursor = 7;
                }
                gPlayNonpositionalSoundEffect(0x47, 0);
            }
            break;
        case 0:
            y = arg2;
            if (gPlayerMenuDepth[s2] != 0 && arg4 == RAW[0x924]) {
                s32 *font;
                s32 *pi;
                s32 col;
                s32 rgb;
                gMeasureTextExtent(gProfileNameCellWidthText, &w, &h);
                w = w + 1;
                arg2 = 0;
                font = gGetActiveFont();
                arg3 = (s32)(gNameEntryGlyphBuf + s2 * 8);
                s2 = (s32)&gScalarCursor[s2];
                h = ((s16 *)font)[3];
                txt[1] = 0;
                n = arg1 - (w + 1) * 4;
                render_loop:
                {
                    txt[0] = gNameEntryAlphabet[((u8 *)arg3)[arg2]];
                    gMeasureTextExtent(txt, &w2, &h2);
                    if (arg2 == *(s32 *)s2) {
                        dst = (u8 *)n;
                        one = 1;
                        savedBase = gHighlightShadeTable;
                        step = gGameClock;
                        heightTmp = h;
                        __asm__("sra %0,%0,4\n\tandi %0,%0,7\n\taddu %0,%0,%1"
                            : "+r"(step) : "r"(savedBase), "r"(heightTmp), "r"(dst), "r"(one));
                        selected = *(u8 *)step;
                        step = 10;
                        __asm__("" : "+r"(selected), "+r"(step));
                        selected += 0x80;
                        gEmitFlatQuadRect((s32)dst, y + heightTmp + 2, w, one, step,
                                      selected, selected, selected);
                        col = 8;
                    } else {
                        col = 15;
                    }
                    arg2++;
                    gDrawAlignedString(txt, n + (w - w2) / 2, y, col);
                    lenv = w;
                    __asm__("addiu %0,%1,1" : "=r"(step) : "r"(n), "r"(lenv));
                    n = step + lenv;
                    if (arg2 < 8) goto render_loop;
                }
            } else {
                if (RAW[arg4 * 0x70 + 0x428] == 0) {
                    q = gLookupTextString(0x3714);
                    if (q == 0 || func_8008CCC0(q) >= 9) {
                        q = gDefaultNameString;
                    }
                } else {
                    q = RAW + arg4 * 0x70 + 0x428;
                }
                gDrawAlignedMultilineText(q, arg1, arg2, arg3, 0, 4, 8, 0);
            }
            break;
        }
    }
}
