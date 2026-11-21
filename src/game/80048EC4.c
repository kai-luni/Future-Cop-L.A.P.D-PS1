// Categories: Graphics > Text | Game Engine > Init
#include "game/types.h"

struct FontEntry;

extern u16 gTexpageTable[] __asm__("D_800A1940");
extern u16 gPaletteTable[] __asm__("D_800A1970");
extern u32 gEffectId[] __asm__("D_8009FACC");
extern u16 gTexBankVramY __asm__("D_8009F76A");
extern u16 gLoadedFont __asm__("D_8009F768");
extern struct FontEntry *gFont __asm__("D_8009F35C");
extern s32 gRampFillCounter __asm__("D_8009F360");
extern s32 gFontBankCount __asm__("D_8009F364");

extern void func_80048978(s32 blue, s32 green, s32 red);
void gInitTextRendererState(void) __asm__("func_80048EC4");


/**
 * @brief Reinitialises the text renderer state, clears both 20-entry colour tables, and adds white as entry 0.
 *
 * Resets font and palette tracking globals, zeroes the 20-element u16 tables D_800A1940
 * and D_800A1970, and allocates palette slot 0 with RGB (0x1F, 0x1F, 0x1F) via func_80048978.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F76A (set to 1), D_8009F768/D_8009F35C/D_8009F360/D_8009F364/D_8009FACC[0] (cleared to 0);
 * D_800A1940/D_800A1970 - 0x14-entry u16 colour/palette tables cleared in a loop.
 * @par Control flow
 * Initialises two flags, runs a 20-iteration for loop (i = 0..0x13), clears four globals, and invokes func_80048978.
 * @par Calls
 * func_80048978 - registers default white colour (0x1F, 0x1F, 0x1F) via func_800488A4 and increments D_8009F360.
 * @par Side effects
 * Overwrites text colour tables, font count, active font pointer, palette cursor, and entry 0 palette data.
 * @par Callers
 * Confirmed: func_8004783C (HUD/text layer reset).
 */
void gInitTextRendererState(void)
{
    s32 i;

    gTexBankVramY = 1;
    gLoadedFont = 0;
    for (i = 0; i < 0x14; i++) {
        gTexpageTable[i] = 0;
        gPaletteTable[i] = 0;
    }
    gFont = 0;
    gRampFillCounter = 0;
    gFontBankCount = 0;
    gEffectId[0] = 0;
    func_80048978(0x1F, 0x1F, 0x1F);
}
