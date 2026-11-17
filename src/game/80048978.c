// Categories: Graphics > Text | Game Engine > Init
#include "game/types.h"

extern s32 gRampFillCounter __asm__("D_8009F360");

extern void func_800488A4(s32 index, u8 blue, u8 green, u8 red);

/**
 * @brief Appends one colour to the text renderer's 20-entry palette.
 *
 * Until the fill counter D_8009F360 reaches 0x14 the colour is written through
 * func_800488A4 at the next free index and the counter advances. Once the table
 * is full the call does nothing, so extra colours are dropped instead of
 * overwriting earlier entries.
 *
 * @param blue Blue channel.
 * @param green Green channel.
 * @param red Red channel (channel order is the renderer's).
 *
 * @par Globals
 * D_8009F360 - number of palette entries in use.
 *
 * @par Control flow
 * Single bound test.
 *
 * @par Calls
 * func_800488A4 - writes one palette entry.
 *
 * @par Side effects
 * Fills palette slot N and increments the counter.
 *
 * @par Callers
 * Unknown: no direct edge; reached from the text/front-end setup paths.
 */
void func_80048978(s32 blue, s32 green, s32 red)
{
    if (gRampFillCounter < 0x14) {
        func_800488A4(gRampFillCounter, blue, green, red);
        gRampFillCounter++;
    }
}
