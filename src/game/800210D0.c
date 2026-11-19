// Categories: Graphics > Text | Resources > Save Data
#include "game/types.h"

extern u16 gPunctuationTable[] __asm__("D_8009AB18");
struct Range { u16 base, start; };
extern struct Range gCharRangeTable[] __asm__("D_8009AB0C");

u16 gEncodeCardTitleGlyph(u8 c) __asm__("func_800210D0");

/**
 * @brief Encodes a printable ASCII byte as a byte-swapped Shift-JIS glyph
 * code.
 *
 * @param c ASCII input byte.
 * @return Byte-swapped glyph code, or zero for unsupported input.
 *
 * @par Confirmed mechanics
 * Classifies ASCII ranges, uses a halfword lookup for punctuation and
 * base-minus-start arithmetic for digits and letters, then swaps the two bytes
 * of the result.
 *
 * @par Globals
 * Reads gPunctuationTable and gCharRangeTable.
 *
 * @par Callers
 * Direct calls in the retail listing are from gEncodeProfileNameGlyphs.
 *
 * @par Role and path
 * The save-title builder calls this for the encoded text placed in the card
 * header.
 *
 * @par Unknown
 * The routine handles ASCII input and contains no multibyte decoder.
 */
u16 gEncodeCardTitleGlyph(u8 c)
{
    register int group __asm__("$5") = 0;
    register u8 offset __asm__("$3") = 0;
    register u32 value __asm__("$3");
    register u32 raw __asm__("$2");
    char frame;

    if ((u8)(c - 0x20) < 0x10) offset = 1;
    else if ((u8)(c - 0x30) < 0xA) { }
    else if ((u8)(c - 0x3A) < 7) offset = 0xB;
    else if ((u8)(c - 0x41) < 0x1A) group = 1;
    else if ((u8)(c - 0x5B) < 6) offset = 0x25;
    else if ((u8)(c - 0x61) < 0x1A) group = 2;
    else if ((u8)(c - 0x7B) < 4) offset = 0x3F;
    else return 0;

    __asm__ volatile ("" : "=m" (frame));
    if (offset != 0) {
        register unsigned int input __asm__("$2") = c;
        register int index __asm__("$3");
        __asm__ ("" : "+r" (input));
        index = offset + 0x1F;
        __asm__ ("" : "+r" (index));
        raw = gPunctuationTable[input - index];
    }
    else {
        register struct Range *base __asm__("$2") = gCharRangeTable;
        register struct Range *range __asm__("$3");
        __asm__ ("" : "+r" (base));
        range = (struct Range *)((group << 2) + (u32)base);
        __asm__ ("" : "+r" (range));
        raw = range->base + c - range->start;
    }
    value = raw;
    __asm__ ("" : "+r" (value));
    {
        register u32 high __asm__("$2") = (value >> 8) & 0xFF;
        value <<= 8;
        __asm__ ("" : "+r" (high), "+r" (value));
        value |= high;
    }
    __asm__ ("" : "+r" (value));
    return value;
}
