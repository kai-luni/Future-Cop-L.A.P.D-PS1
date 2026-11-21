// Categories: Graphics > Text > Rendering | Game Engine > Front-End Configuration
#include "game/types.h"

struct State27F80 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
};

struct MenuText27F80 {
    u8 *text[0x40];
};

extern struct State27F80 *gScratchBlockPtr __asm__("D_8009F68C");
extern struct MenuText27F80 *gResultsTextTable __asm__("D_8009F2C0");
extern u8 gResultsDecimalFormat[] __asm__("D_8009F2FC");
extern u8 gTimeMmSsFormat[] __asm__("D_80010DCC");

extern s32 gGetPlayerResultsStat(void *player, s32 arg1) __asm__("func_80059E9C");
extern s32 func_80049340(u8 *text);
extern void gDrawAlignedString(u8 *text, s32 x, s32 y, s32 color) __asm__("func_80049100");
extern void func_8008CD30(u8 *dest, u8 *fmt, ...);

void gDrawResultsTextBlock(void *player, u8 *text, s32 x, s32 color) __asm__("func_80027F80");

/**
 * @brief Draws a control-coded results text block with centered labels and formatted player
 * statistics.
 *
 * @param player Player block forwarded to the stat lookup.
 * @param text Control-coded string: 0x1B terminates, 0x1A tightens the line (y -= 4), 0x19
 * skips a line, bytes >= 0x40 are stat codes (minus 0x40), other bytes index the menu text
 * table.
 * @param x Centre column.
 * @param color Text color (low 16 bits).
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Starts y at 0x21, moves by nine per input byte, subtracts four for 0x1A, skips drawing for
 * 0x19 and stops at 0x1B. Codes starting at 0x40 resolve or reuse cached stat values. Formats
 * IDs 15 and above as minutes and seconds after (value + 150) divided by 300. For each drawn
 * line, measures text and positions it at x minus width divided by four.
 *
 * @par Globals
 * Reads gResultsTextTable, gResultsDecimalFormat and gTimeMmSsFormat. Reads and updates the
 * stat cache through gScratchBlockPtr.
 *
 * @par Callers
 * Direct calls from gDrawPlayerResultsScreen (func_80028164).
 *
 * @par Role and path
 * The results screen draws its static text and both player stat columns through this decoder.
 *
 * @par Unknown
 * The full vocabulary of stat IDs is unresolved.
 */
void gDrawResultsTextBlock(void *player, u8 *text, s32 x, s32 color)
{
    s32 c;
    u8 *str;
    s32 v;
    s32 t;
    s32 y;
    s32 i;
    u8 buf[0x10];

    y = 0x21;
    i = 0;
    c = text[0];
    if (c == 0x1B) {
        return;
    }
    do {
        if (c == 0x1A) {
            y -= 4;
        } else if (c != 0x19) {
            if (c >= 0x40) {
                c -= 0x40;
                if (gScratchBlockPtr->unk0 == 0) {
                    t = gGetPlayerResultsStat(player, c);
                    ((s32 *)gScratchBlockPtr)[gScratchBlockPtr->unk4] = t;
                } else {
                    t = ((s32 *)gScratchBlockPtr)[gScratchBlockPtr->unk4];
                }
                gScratchBlockPtr->unk4 = gScratchBlockPtr->unk4 + 1;
                if (c >= 0xF) {
                    t += 0x96;
                    t /= 300;
                    func_8008CD30(buf, gTimeMmSsFormat, t / 60, t % 60);
                } else {
                    func_8008CD30(buf, gResultsDecimalFormat, t);
                }
                str = buf;
            } else {
                str = gResultsTextTable->text[c];
            }
            v = func_80049340(str);
            if (v < 0) {
                v += 3;
            }
            gDrawAlignedString(str, x - (v >> 2), y, color & 0xFFFF);
        }
        i++;
        c = text[i];
        y += 9;
    } while (c != 0x1B);
}
