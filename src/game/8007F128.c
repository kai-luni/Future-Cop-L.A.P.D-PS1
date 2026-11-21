// Categories: Graphics > HUD > Code Entry | Utilities > Character Table
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

extern s32 gApplyPasswordUnlocks(void) __asm__("func_8007EA78");
extern s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1) __asm__("func_80046C84");
extern void func_80078740(void);
extern void func_800493C0(u32 index, u8 blue, u8 green, u8 red);
extern void func_800788F0(u8 *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7);
extern void func_80078B34(s32 arg0, s32 arg1, s32 arg2);
extern void gEmitFlatQuadRect(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7) __asm__("func_80082EF0");
extern void gDrawAlignedString(u8 *text, s32 x, s32 y, s32 color) __asm__("func_80049100");
extern void func_800492A0(u8 *text, s32 *width, s32 *height);
extern struct FontEntry *func_80048E88(void);
extern void gDecodeCodeDisplay(s32 input0, s32 input1, s32 input2, s32 input3, s32 arg5, s32 arg6) __asm__("func_8007F128");

extern void gDecodeCodeDisplay(s32 input0, s32 input1, s32 input2, s32 input3, s32 arg5, s32 arg6)
{
    u8 buf[12];
    s32 a0;
    s32 a1;
    s32 a2;
    u32 i;
    u8 *odd;
    u8 *even;
    u8 *dst;
    u8 *src;
    u8 *letter;

    s32 a3;

    a3 = input3;
    a0 = input0;
    a1 = input1;
    a2 = input2;
    i = 0;
    odd = gOctalDigitTable;
    even = gHexDigitTable;
    dst = buf;
    src = gPackedPasswordBytes;
    do {
        if (i & 1) {
            letter = (u8 *)(((*src + i) & 7) + (u32)odd);
        } else {
            letter = (u8 *)(((*src + i) & 0xF) + (u32)even);
        }
        *dst++ = *letter;
/**
 * @brief Decodes the ten stored code bytes into a display string and draws it.
 *
 * @param input0, input1, input2 x, y and colour forwarded to the text draw.
 * @param input3 Font parameter forwarded as the last draw argument.
 * @param arg5, arg6 alignX/alignY forwarded to the text draw.
 * @return Nothing.
 *
 * @par Confirmed
 * For each of the ten bytes at D_800A3938: even positions index the 16-entry
 * table D_8009C1AC with (byte + i) & 0xF, odd positions the 8-entry table
 * D_8009C1BC with (byte + i) & 7. The resulting characters are NUL-terminated
 * in a 12-byte buffer and drawn via func_800788F0(buf, x, y, colour, 0,
 * alignX, alignY, font).
 *
 * @par Likely role
 * The password display of the code-entry screen func_8007F1F8 and the
 * profile screen func_80080038: the two tables are the 16+8 symbol alphabet
 * of the save-code system, position-dependent so the same byte shows
 * different glyphs in even/odd cells.
 *
 * @par Unknown
 * Whether the +i rotation is obfuscation or part of the code checksum.
 *
 * @par Callers
 * func_8007F1F8, func_80080038.
 */
        __asm__ volatile("");
        i++;
        src++;
    } while (i < 10);
    buf[i] = 0;
    func_800788F0(buf, a0, a1, a2, 0, arg5, arg6, a3);
}
