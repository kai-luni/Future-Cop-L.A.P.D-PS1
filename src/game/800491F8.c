// Categories: Graphics > HUD | Graphics > Text > Rendering | Utilities > Formatting
#include "game/types.h"

extern char D_8009F368[];

extern s32 func_8008CD30(char *buffer, char *format, s32 value);
extern void func_80049100(char *text, s32 x, s32 y, s32 color);

/**
 * @brief Formats a number with the shared format string and draws it as
 *        HUD text.
 *
 * @param value Number to format.
 * @param x Screen x.
 * @param y Screen y.
 * @param color Text color; only the low 16 bits are used.
 * @return Nothing.
 *
 * @par Confirmed
 * func_8008CD30(buffer, D_8009F368, value) formats into a 0x20-byte stack
 * buffer using the global format string; color &= 0xFFFF;
 * func_80049100(buffer, x, y, color) draws the text.
 *
 * @par Callers
 * func_8006C080 (status bar).
 */
void func_800491F8(s32 value, s32 x, s32 y, s32 color)
{
    char buffer[0x20];

    func_8008CD30(buffer, D_8009F368, value);
    color &= 0xFFFF;
    func_80049100(buffer, x, y, color);
}
