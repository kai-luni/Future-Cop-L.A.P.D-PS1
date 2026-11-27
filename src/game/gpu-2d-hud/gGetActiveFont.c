// Categories: Graphics > Text > Font
#include "game/types.h"

struct FontEntry;

extern struct FontEntry *gFont __asm__("D_8009F35C");
struct FontEntry *gGetActiveFont(void) __asm__("func_80048E88");


/**
 * @brief Returns the currently active font entry.
 *
 * @return The global font pointer D_8009F35C, or 0 when no font is installed.
 *
 * @par Confirmed
 * Single load of D_8009F35C and return; no side effects. The same pointer is
 * read directly by the text measurement and rendering helpers.
 *
 * @par Likely role
 * Accessor used by the front-end widgets so they can query the active font
 * without touching the global themselves.
 *
 * @par Callers
 * func_800788F0, func_8007ABBC, func_8007D7CC, func_8007F1F8, func_8008125C.
 */
struct FontEntry *gGetActiveFont(void)
{
    return gFont;
}
