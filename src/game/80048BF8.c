// Categories: Graphics > Text > Font Upload | Graphics > VRAM Transfer
#include "game/types.h"

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Rect;

struct FontInfo {
    u8 pad00[4];
    s16 f04;
    s16 f06;
    u8 pad08[8];
    u8 data[1];
};

struct FontEntry {
    s16 f00;
    s16 f02;
    s16 f04;
    s16 f06;
    s16 f08;
    union {
        u16 u;
        s16 s;
    } f0A;
    s16 f0C;
    s16 f0E;
    s32 f10;
    struct FontInfo *info;
    s32 f18;
    u8 data[0x100];
};

extern struct FontEntry gGlyphEntryTable[] __asm__("D_800A19A0");
extern u16 gPaletteTable[] __asm__("D_800A1970");
extern u16 gTexpageTable[] __asm__("D_800A1940");
extern u16 D_800A1E10[];
extern u16 gTexBankVramY __asm__("D_8009F76A");
extern u16 gLoadedFont __asm__("D_8009F768");

extern void gLoadImage(Rect *rect, void *data) __asm__("func_80092F38");
extern void func_80092C8C(s32 arg0);
void gLoadFontAsset(s32 arg0) __asm__("func_80048BF8");


/**
 * @brief Uploads one font bank into the texture area and builds its texpage
 *        word.
 *
 * @param arg0 Bank index; anything >= 4 is ignored.
 * @return Nothing.
 *
 * @par Confirmed flow
 * 1. Guard: the font's row count D_8009F76A plus the bank height (info->f06)
 *    must stay below 0x1FF.
 * 2. The glyph bitmap at info + 0x10 is nibble-swapped in place (the font data is
 *    stored with 4-bit packed pixels in the opposite order).
 * 3. The bank is placed at VRAM x = 0x380 and y = D_8009F76A + 0x100, uploaded
 *    with func_80092F38 as (w/4, height) and synchronised with func_80092C8C(0).
 * 4. D_800A1940[arg0] receives the texpage word built from the destination row:
 *    bits 8/10 select the VRAM column half, 0xE the texture page.
 * 5. For bank 0 an extra 0x10x1 rectangle at (0x380, 0x1FF) is written - the
 *    "blank" glyph the text renderer uses for spaces.
 *
 * @par Callers
 * func_80048DEC (the font selector).
 *
 * @par Environment path
 * Uploads a font bank's glyph bitmap to VRAM (0x1E0, 0) and palette to (0,
 * 0x1F0) via func_80092F38/func_80092C8C, computes its texpage word via
 * func_80092230, and populates the font glyph/palette tables (called by
 * func_80048A14 and func_80048DEC).
 */
void gLoadFontAsset(s32 arg0)
{
    struct FontInfo *info;
    s32 size;
    s32 i;
    u8 val;
    Rect rect[2];
    u8 *data;

    if (arg0 < 4) {
        info = gGlyphEntryTable[arg0].info;
        if (gTexBankVramY + info->f06 < 0x1FF) {
            size = (info->f04 * info->f06) / 2;
            for (i = 0; i < size; i++) {
                val = i[(u8 *)info + 0x10];
                i[(u8 *)info + 0x10] = ((val & 0xF) << 4) | (val >> 4);
            }

            gPaletteTable[arg0] = gTexBankVramY;
            gTexBankVramY += info->f06;
            rect[0].x = 0x380;
            rect[0].y = gPaletteTable[arg0] + 0x100;
            data = (u8 *)info + 0x10;
            rect[0].w = info->f04 / 4;
            rect[0].h = info->f06;
            gLoadImage(&rect[0], data);
            func_80092C8C(0);

            gTexpageTable[arg0] = (((gPaletteTable[arg0] + 0x100) & 0x100) >> 4) | 0xE | (((gPaletteTable[arg0] + 0x100) & 0x200) << 2);

            if (arg0 == 0) {
                rect[1].x = 0x380;
                rect[1].y = 0x1FF;
                rect[1].w = 0x10;
                rect[1].h = 1;
                gLoadImage(&rect[1], D_800A1E10);
                func_80092C8C(0);
                gLoadedFont = 0x7FF8;
            }

            for (i = 0; i < size; i++) {
                val = i[(u8 *)info + 0x10];
                i[(u8 *)info + 0x10] = ((val & 0xF) << 4) | (val >> 4);
            }
        }
    }
}
