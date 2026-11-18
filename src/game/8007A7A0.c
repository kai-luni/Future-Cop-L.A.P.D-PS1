// Categories: Game Engine > Front-End Configuration | Graphics > HUD
#include "game/types.h"

struct MenuState {
    u8 pad00[0xC];
    s32 mode;
    u8 pad10[0x4C];
    s32 active;
};

extern struct MenuState *gGameSession[] __asm__("D_8009FA94");
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern u8 gPlayerWrapLimit __asm__("D_8009F3C1");
extern s32 gPlayerSelection[] __asm__("D_8009F8F0");
extern u16 gLabelIdTable[] __asm__("D_8009BEEC");
extern s32 func_80046C84(s32 id, s32 arg1);
extern void func_800788F0(u8 *text, s32 x, s32 y, s32 color, s32 textId,
                         s32 alignX, s32 alignY, s32 font);

/**
 * @brief Menu selector widget: steps a per-player option value with
 *        wraparound and draws the selected label.
 *
 * @param command 0 draw, 1 step down, 2 step up.
 * @param x Draw column (forced to 0xA0 outside the two-player mode-2 view).
 * @param y Draw row; the label lands at y + 0xC.
 * @return Nothing.
 *
 * @par Confirmed
 * The player index is D_8009F8B8 when D_8009FA94[0]->active is set, else 0.
 * Steps play sound 0x47 and move D_8009F8F0[player]: down wraps to 3 (or 2
 * when D_8009F3C1 is 0), up wraps to 0 above 3 (or 2). Draw maps the value
 * 2/1/0/3 to label ids 0/1/2/3 in D_8009BEEC and renders via func_800788F0
 * (colour 0xF, alignX 4, size 8, font 1); the label is suppressed for
 * player 1 unless active == 1 and mode == 2.
 *
 * @par Likely role
 * A three/four-way option selector (the D_8009F3C1 flag likely enables a
 * fourth entry); the inverted label order suggests the values count down
 * while the labels read up.
 *
 * @par Callers
 * No direct callers in the call graph and no address reference found;
 * dispatch unconfirmed (likely a widget table entry).
 */
void gStepPlayersOptionPane(s32 command, s32 x, s32 y) __asm__("func_8007A7A0");

void gStepPlayersOptionPane(s32 command, s32 x, s32 y)
{
    s32 index;
    s32 textId;

    switch (command) {
    case 1:
    {
        register s32 *entry __asm__("$3");
        index = 0;
        if (gGameSession[0]->active != 0) {
            index = gActivePlayerIndex;
        }
        func_80046C84(0x47, 0);
        entry = gPlayerSelection;
        entry = (s32 *)((index << 2) + (s32)entry);
        (*entry)--;
        if (*entry < 0) {
            if (gPlayerWrapLimit != 0) {
                *entry = 3;
            } else {
                *entry = 2;
            }
        }
        break;
    }
    case 2:
    {
        register s32 *base __asm__("$3");
        register s32 *entry __asm__("$4");
        register s32 value __asm__("$2");
        register s32 offset __asm__("$2");
        s32 flag;
        s32 below;
        index = 0;
        if (gGameSession[0]->active != 0) {
            index = gActivePlayerIndex;
        }
        func_80046C84(0x47, 0);
        base = gPlayerSelection;
        offset = index << 2;
        entry = (s32 *)(offset + (s32)base);
        value = *entry;
        flag = gPlayerWrapLimit;
        value++;
        *entry = value;
        if (flag != 0) {
            below = value < 4;
        } else {
            below = value < 3;
        }
        if (!below) {
            *entry = 0;
        }
        break;
    }
    case 0:
    {
        register s32 *base __asm__("$3");
        register u16 *labels __asm__("$3");
        register u8 *text __asm__("$4");
        register s32 selection __asm__("$2");
        selection = gActivePlayerIndex;
        __asm__("" : "+r"(selection));
        base = gPlayerSelection;
        __asm__("" : "+r"(base));
        switch (base[selection]) {
        case 2:
            textId = 0;
            break;
        case 1:
            textId = 1;
            break;
        case 0:
            textId = 2;
            break;
        case 3:
            textId = 3;
            break;
        default:
            textId = -1;
            break;
        }
        if (gGameSession[0]->active != 1 || gGameSession[0]->mode != 2) {
            if (gActivePlayerIndex != 0) {
                textId = -1;
            }
            x = 0xA0;
        }
        if (textId != -1) {
            text = 0;
            __asm__("" : "+r"(text));
            labels = gLabelIdTable;
            __asm__("" : "+r"(labels));
            func_800788F0(text, x, y + 0xC, 0xF, labels[textId], 4, 8, 1);
        }
        break;
    }
    }
}
