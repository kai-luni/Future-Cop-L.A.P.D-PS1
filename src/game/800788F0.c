#include "game/types.h"

struct FontEntry {
    u8 unk00[6];
    s16 height;
};

extern s8 D_8009F8BC;

extern u8 *func_80089914(s32 id);
extern s32 func_80048E28(s32 index);
extern struct FontEntry *func_80048E88(void);
extern void func_80049100(u8 *text, s32 x, s32 y, s32 color);
extern void func_800492A0(u8 *text, s32 *width, s32 *height);
extern u8 *func_8008CCD0(u8 *str, s32 c);
extern s32 func_8008CCC0(u8 *str);

void func_800788F0(u8 *text, s32 x, s32 y, s32 color, s32 textId,
                   s32 alignX, s32 alignY, s32 font)
{
    register s32 ay __asm__("$21");
    register s32 font0 __asm__("$16");
    register s32 y0 __asm__("$20");
    register s32 lineCount __asm__("$23");
    register s32 line __asm__("$22");
    s32 oldFont;
    s32 drawX;
    s32 drawY;
    s32 width;
    s32 height;
    s32 lineHeight;
    u8 *p;

    ay = alignY;
    font0 = font;
    y0 = y;
    lineCount = 0;
    line = 1;
    oldFont = -1;

    if (text == 0) {
        if (textId != 0) {
            text = func_80089914(textId & 0xFFFF);
        }
        if (text == 0) {
            return;
        }
    }

    if (D_8009F8BC != font0) {
        oldFont = D_8009F8BC;
        func_80048E28(font0);
    }

    if (ay == 0x10 && text != 0) {
        p = text;
        lineCount++;
        do {
            p = func_8008CCD0(p + 1, '\n');
            lineCount++;
        } while (p != 0);
        lineCount--;
    }

    while (*text != 0) {
        p = func_8008CCD0(text, '\n');
        if (p != 0) {
            *p = 0;
        }

        drawX = x;
        drawY = y0;
        if (ay == 0x10) {
            lineHeight = func_80048E88()->height;
            drawY = y0 - (lineCount - line) * lineHeight;
        }

        if (alignX != 1 || ay != 8) {
            func_800492A0(text, &width, &height);
            if (alignX == 2) {
                drawX -= width;
            } else if (alignX == 4) {
                drawX -= width / 2;
            }
            if (ay == 0x10) {
                drawY -= height;
            } else if (ay == 0x20) {
                drawY -= height / 2;
            }
        }

        func_80049100(text, drawX, drawY, (u16) color);

        if (p != 0) {
            text = p + 1;
            *p = '\n';
            {
                register s32 advance __asm__("$2");

                advance = func_80048E88()->height;
                line++;
                y0 += advance;
            }
        } else {
            text += func_8008CCC0(text);
        }
    }

    if (oldFont != -1) {
        func_80048E28(oldFont);
    }
}
