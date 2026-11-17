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
extern struct MenuText27F80 *D_8009F2C0;
extern u8 D_8009F2FC[];
extern u8 gTimeMmSsFormat[] __asm__("D_80010DCC");

extern s32 func_80059E9C(void *player, s32 arg1);
extern s32 func_80049340(u8 *text);
extern void func_80049100(u8 *text, s32 x, s32 y, s32 color);
extern void func_8008CD30(u8 *dest, u8 *fmt, ...);

/**
 * @brief Renders a control-coded menu text block: line feeds, stat lookups
 *        and time formatting, centred per line.
 *
 * @param player Player block forwarded to the stat lookup.
 * @param text Control-coded string: 0x1B terminates, 0x1A tightens the line
 *        (y -= 4), 0x19 skips a line, bytes >= 0x40 are stat codes (minus
 *        0x40), other bytes index the menu text table.
 * @param x Centre column.
 * @param colour Text colour (low 16 bits).
 * @return Nothing.
 *
 * @par Confirmed
 * Stat codes resolve through func_80059E9C(player, code) when the cache
 * D_8009F68C->unk0 is 0 (the result is cached at the running index unk4),
 * else the cached word is reused. Codes >= 0xF format as time: (value +
 * 0x96) / 300, then mm:ss via D_80010DCC; smaller codes format decimal via
 * D_8009F2FC. Plain codes take D_8009F2C0->text[c]. Each line is measured
 * with func_80049340 (negative widths biased by +3) and drawn centred via
 * func_80049100 at x - (width >> 2); y starts at 0x21 and gains 9 per line.
 *
 * @par Likely role
 * The results/statistics screen renderer of func_80028164: the control codes
 * embed live player stats and mission times into the layout text.
 *
 * @par Unknown
 * The exact stat code map behind func_80059E9C.
 *
 * @par Callers
 * func_80028164 (five call sites, verified in the retail listing; the
 * call-graph edges are missing).
 */
void func_80027F80(void *player, u8 *text, s32 x, s32 color)
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
                    t = func_80059E9C(player, c);
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
                    func_8008CD30(buf, D_8009F2FC, t);
                }
                str = buf;
            } else {
                str = D_8009F2C0->text[c];
            }
            v = func_80049340(str);
            if (v < 0) {
                v += 3;
            }
            func_80049100(str, x - (v >> 2), y, color & 0xFFFF);
        }
        i++;
        c = text[i];
        y += 9;
    } while (c != 0x1B);
}
