// Categories: Game Engine > Front-End > List Screen | Input > Menu Navigation | Game Engine > Front-End Configuration
#include "game/types.h"

typedef struct {
    s32 unk0;
    s32 unk4;
    u8 pad8[4];
    s16 unkC;
    u8 unkE;
    u8 padF[0x10];
    u8 unk1F;
    u8 pad20[4];
} Entry;
typedef struct {
    u8 pad0[0xC];
    s32 unkC;
    u8 pad10[0x3C];
    s32 unk4C;
    u8 pad50[0xC];
    s32 unk5C;
} State;
extern State *gGameSession __asm__("D_8009FA94");
extern Entry gWideTextTable[] __asm__("D_8009B4E4");
extern s8 gPlayerMenuDepth[] __asm__("D_8009F8C0");
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern s8 gMissionListMinIndex __asm__("D_8009F881");
extern s8 gMissionListMaxIndex __asm__("D_8009F882");
extern s8 gSelectedMissionIndex __asm__("D_8009F880");
extern s8 gLineCount __asm__("D_8009F87F");
extern s8 gSelectedRow __asm__("D_8009F87E");
extern s8 *gMissionListTablePtr __asm__("D_8009F884");
extern u8 gUiHoldFlag __asm__("D_8009F8BD");
extern s32 gPendingRowCapture __asm__("D_8009F8C4");
extern s32 gDeferredTransition __asm__("D_8009F8C8");
extern u8 gLax1ModuleName[] __asm__("D_8009F444");
extern u8 gLax2ModuleName[] __asm__("D_8009F488");
extern u8 gFrontEndModuleName[] __asm__("D_8009B300");
extern u8 gOnePlayerLoadoutMenuDescriptor[] __asm__("D_800A3628");
extern u8 gTwoPlayerLoadoutMenuDescriptor[] __asm__("D_800A3658");
extern u8 gScreenDescriptor[] __asm__("D_800A3838");
extern u8 *gPendingModuleNameFlag __asm__("D_8009F3EC");
extern u8 gModuleLoadState __asm__("D_8009F3F0");
extern u8 *gFontBankTable __asm__("D_8009F468");
extern void gActivateMenuPanel(void *, s32) __asm__("func_80078650");
extern s32 gPlayNonpositionalSoundEffect(s32, s32) __asm__("func_80046C84");
extern void func_8008CCB0(void *, s32);
extern s32 func_8008CCA0(void *, void *);
extern void func_80078B34(s32, s32, s32);
extern void gDrawProfileSlotList(s32) __asm__("func_8007B460");
#define SELECTED (gWideTextTable[gMissionListTablePtr[gSelectedMissionIndex]])

/**
 * @brief Item handler of the profile-list screen: confirm opens the level list,
 *        left/right and up/down move profile or level, the draw message queues
 *        the screen prompts.
 *
 * @param arg0 Message id. 0 draws, 1 and 5 advance (right/down), 2 and 4 step
 *        back (left/up), 3 confirms. The front-end driver calls the handler
 *        with five arguments (message, x, y, colour, item index) but this body
 *        only reads the message.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * The selected row is D_8009F884[D_8009F880] into the profile table
 * D_8009B4E4, and the per-player edit depth is the byte
 * D_8009F8C0[D_8009F8B8]. The window bounds are D_8009F881/D_8009F882, the
 * level index inside a row is D_8009F87E with D_8009F87F as its upper limit,
 * and D_8009F8C8/D_8009F8C4 are the pending-transition pair the driver reads
 * after the item pass. The row value at +0x1F behaves as that row's level count:
 * func_8007BCF4 draws the numbered lines 1 .. D_8009F87F + 1 for a type-2 row.
 * Message 3 with a zero depth byte just sets it to 1, i.e. arms editing.
 * Otherwise, when the global state block D_8009FA94 has +0x5C == 1, a depth of
 * 1 with +0x0C == 1 and a non-zero row value at +0x1F promotes the depth to 2,
 * clamps that row value into 0..9, mirrors it into D_8009F87F and D_8009F87E and
 * clears D_8009F8BD; every other case copies the row value (or D_8009F87E at
 * depth 2) into D_8009FA94 +0x4C, the menu cursor func_80076A2C consumes. In the
 * same +0x5C == 1 case the level screen is pushed through func_80078650 -
 * D_800A3628 when +0x0C == 1, D_800A3658 otherwise - with D_8009F8C8 = 2 and
 * D_8009F8C4 = 0, and a row value of 10 (the last entry) captures the row byte
 * +0x0E as a special id.
 * Without +0x5C == 1 the transition is only deferred: D_8009F8C8 = 1 and
 * D_8009F8C4 = the row's +0x04 word.
 * Whenever D_8009F8C8 became non-zero the row name at +0x00 is copied into
 * D_8009B300 with func_8008CCB0 and compared against D_8009F444 with
 * func_8008CCA0; a match selects D_8009F488 as D_8009F3EC, otherwise
 * D_8009F3EC is cleared. D_8009F3F0 receives the special id if there was one,
 * else the row's +0x0C byte.
 * Messages 1/5 and 2/4 only act when the row is present: they first play sound
 * 0x46 through func_80046C84 and then, when the row type D_8009F8C0[D_8009F8B8]
 * is 2 (a level sub-list), step D_8009F87E within 0..D_8009F87F and otherwise
 * step the active row index D_8009F880 within D_8009F881..D_8009F882; both wrap
 * to the opposite end of their window.
 * Message 0, when this panel is the current front-end screen (D_8009F468 ==
 * D_800A3838), queues the display ids 0x44C (slot 0) and 0x10CC (slot 1) with
 * func_80078B34 and redraws the profile body with func_8007B460(D_8009F8B8).
 *
 * @par Likely role
 * One widget of the front-end list screen: the panel records of the item table
 * that func_80077AD4 registers carry this address at +0x04, and func_80078DD0
 * dispatches messages 1..5 to it, so the semantics 1 = right, 2 = left,
 * 4 = up, 5 = down, 3 = accept are the driver's, not this function's guess.
 * The +0x5C and +0x0C words of D_8009FA94 are the split-screen flags the
 * neighbours use: func_8007B460 paints the two-player layout only while both are
 * 1, and +0x0C == 1 is also what turns the confirm here into a depth promotion
 * instead of a plain value store.
 * The strcpy/strcmp pair looks like the "which module does this profile load"
 * decision: D_8009F3EC is the pending module name (func_80076A2C appends .mis
 * to it) and D_8009F3F0 the loading-screen index (func_80025370 maps it onto
 * the 23-entry .lsc table), so confirming a profile seems to prepare the
 * mission load, and value 10 marks a finished profile whose +0x0E byte selects
 * a different artwork.
 *
 * @par Unknown
 * What the two display ids 0x44C/0x10CC render, the meaning of the row bytes
 * +0x04/+0x0C besides being the transition payload and a screen index, and what
 * the two string literals D_8009F444/D_8009F488 contain.
 *
 * @par Callers
 * No direct callers in the call graph. func_80077AD4 registers the panel
 * D_800A3838 with the item table that holds this address, and the item trip of
 * func_80078DD0 reaches it through the handler pointer, so the dispatch is
 * indirect.
 */
void gHandleProfileListItem(s32 arg0) __asm__("func_8007B854");

void gHandleProfileListItem(s32 arg0)
{
    State *state;
    u8 special;
    u8 ready;
    register s8 *p __asm__("$6");
    register void *message __asm__("$4");
    register s8 *base __asm__("$16");
    register s32 index __asm__("$2");
    register s32 firstIndex __asm__("$3");
    u8 limit;
    register s32 value __asm__("$2");
    register Entry *sel __asm__("$4");
    register s32 scaled __asm__("$2");
    register Entry *ep __asm__("$3");
    switch (arg0) {
    case 3:
        firstIndex = gActivePlayerIndex;
        __asm__("" : "+r"(firstIndex));
        p = &gPlayerMenuDepth[firstIndex];
        special = 0;
        if (*p != 0) {
            ready = 0;
            state = gGameSession;
            if (state->unk5C == 1) {
                if (state->unkC == 1) {
                    if (*p == 1) {
                        sel = gWideTextTable;
                        scaled = gMissionListTablePtr[gSelectedMissionIndex] * 36;
                        __asm__("addu %0, %1, %2" : "=r"(ep) : "r"(scaled), "r"(sel));
                        value = ep->unk1F;
                        if (value != 0) {
                            *p = 2;
                            limit = SELECTED.unk1F;
                            gLineCount = limit;
                            if ((s8)limit >= 0) {
                                if ((s8)limit < 10) goto CLAMPED;
                                limit = 9;
                            } else {
                                limit = 0;
                            }
                            gLineCount = limit;
CLAMPED:
                            gSelectedRow = limit;
                            gUiHoldFlag = 0;
                            goto SELECTION_DONE;
                        }
                    } else {
                        value = gSelectedRow;
                    }
                    ready = 1;
                    __asm__("" : "+r"(ready));
                    state->unk4C = value;
SELECTION_DONE:;
                } else {
                    ready = 1;
                }
                if (ready) {
                    if (gGameSession->unkC == 1) message = gOnePlayerLoadoutMenuDescriptor;
                    else message = gTwoPlayerLoadoutMenuDescriptor;
                    gActivateMenuPanel(message, 1);
                    gDeferredTransition = 2;
                    gPendingRowCapture = 0;
                    if (SELECTED.unk1F == 10) special = SELECTED.unkE;
                }
            } else {
                gDeferredTransition = 1;
                gPendingRowCapture = SELECTED.unk4;
            }
            if (gDeferredTransition != 0) {
                func_8008CCB0(gFrontEndModuleName, SELECTED.unk0);
                if (func_8008CCA0(gFrontEndModuleName, gLax1ModuleName) == 0) gPendingModuleNameFlag = gLax2ModuleName;
                else gPendingModuleNameFlag = 0;
                if (special != 0) gModuleLoadState = special;
                else if (SELECTED.unkC != 0) gModuleLoadState = SELECTED.unkC;
            }
        } else {
            *p = 1;
        }
        break;
    case 2:
    case 4:
        index = gActivePlayerIndex;
        __asm__("" : "+r"(index));
        base = gPlayerMenuDepth;
        if (base[index] != 0) {
            gPlayNonpositionalSoundEffect(0x46, 0);
            if (base[gActivePlayerIndex] == 2) {
                gSelectedRow--;
                if (gSelectedRow < 0) gSelectedRow = gLineCount;
            } else {
                gSelectedMissionIndex--;
                if (gSelectedMissionIndex < gMissionListMinIndex) gSelectedMissionIndex = gMissionListMaxIndex;
            }
        }
        break;
    case 1:
    case 5:
        index = gActivePlayerIndex;
        __asm__("" : "+r"(index));
        base = gPlayerMenuDepth;
        if (base[index] != 0) {
            gPlayNonpositionalSoundEffect(0x46, 0);
            if (base[gActivePlayerIndex] == 2) {
                gSelectedRow++;
                if (gSelectedRow > gLineCount) gSelectedRow = 0;
            } else {
                gSelectedMissionIndex++;
                if (gSelectedMissionIndex > gMissionListMaxIndex) gSelectedMissionIndex = gMissionListMinIndex;
            }
        }
        break;
    case 0:
        if (gFontBankTable == gScreenDescriptor) {
            func_80078B34(0x44C, 0, 0);
            func_80078B34(0x10CC, 1, 0);
            gDrawProfileSlotList(gActivePlayerIndex);
        }
        break;
    }
}
