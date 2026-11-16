// Categories: Graphics > Text | Game Engine > Init
#include "game/types.h"

struct FontEntry;

extern u16 D_800A1940[];
extern u16 D_800A1970[];
extern u32 D_8009FACC[];
extern u16 D_8009F76A;
extern u16 D_8009F768;
extern struct FontEntry *D_8009F35C;
extern s32 D_8009F360;
extern s32 D_8009F364;

extern void func_80048978(s32 blue, s32 green, s32 red);

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
void func_80048EC4(void)
{
    s32 i;

    D_8009F76A = 1;
    D_8009F768 = 0;
    for (i = 0; i < 0x14; i++) {
        D_800A1940[i] = 0;
        D_800A1970[i] = 0;
    }
    D_8009F35C = 0;
    D_8009F360 = 0;
    D_8009F364 = 0;
    D_8009FACC[0] = 0;
    func_80048978(0x1F, 0x1F, 0x1F);
}
