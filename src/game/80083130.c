// Categories: Resources > Mission Table | Game Engine > Front-End Module Lookup
#include "game/types.h"

typedef struct {
    char *unk0;
    s32 unk4;
    s32 unk8;
    u8 pad0C[0x15];
    u8 unk21;
    u8 pad22[2];
} Entry;

extern u8 gFrontEndModuleName[] __asm__("D_8009B300");
extern u8 gLax2ModuleName[] __asm__("D_8009F488");
extern u32 gLax1ModuleName[] __asm__("D_8009F444");
extern u8 gCreditsMovieFilename[] __asm__("D_80011E60");
extern Entry gWideTextTable[] __asm__("D_8009B4E4");

extern s32 gBiosStrlen(u8 *) __asm__("func_8008CCC0");
extern u8 gBiosToLower(u8) __asm__("func_8008CCF0");
extern s32 gBiosStrcmp(void *, void *) __asm__("func_8008CCA0");
s32 gFindModuleMovie(void **arg0) __asm__("func_80083130");


/**
 * @brief Resolves the current front-end module name to its movie file and to
 *        the optional fixed overlay name.
 *
 * @param arg0 Out-parameter, may be NULL. Receives "credits.wve" when the
 *             matched table entry has its flag byte set, otherwise NULL.
 *
 * @return The matched entry's @c unk8 pointer (the mission's "po" movie name as
 *         a string pointer), or 0 when the module is not in the table. The
 *         caller treats 0 as "no movie".
 *
 * @par Confirmed flow
 * 1. The module name in D_8009B300 is lowercased with func_8008CCF0 into a
 *    24-byte buffer, stopping at the first '.' (so "FE.mis" becomes "fe",
 *    "m4a1.mis" becomes "m4a1").
 * 2. If the result equals "lax2" (D_8009F488) the five bytes of D_8009F444
 *    ("lax1") are copied over it - "lax2" is an alias of "lax1".
 * 3. The 16-entry table D_8009B4E4 (0x24-byte stride, fields @c name,
 *    @c brfMovie, @c poMovie, @c flags at 0x21) is searched with strcmp.
 *    Entries in retail order: m4a1, ov, lax1, conft, un, m3b, joke, slim, e3,
 *    m2c, hk, m3a, (empty), m1a1, (empty), ovmp.
 * 4. On a hit the function returns entry->poMovie (e.g. "pom4a1.wve") and
 *    writes "credits.wve" (D_80011E60) to *arg0 when entry->flags == 1
 *    (m4a1 and joke). The sibling field entry->brfMovie ("brfm4a1.wve") is
 *    read by other front-end code, not here.
 *
 * @par Likely role
 * Each mission module owns two movies - a briefing ("brf*") and a pre-op
 * cutscene ("po*"). This lookup is what the main loop uses to blit the
 * mission's pre-op frame (the caller wraps it in "Movies\\%s").
 *
 * @par Globals
 * D_8009B300 (module name), D_8009F488 ("lax2"), D_8009F444 ("lax1"),
 * D_8009B4E4 (mission table), D_80011E60 ("credits.wve").
 *
 * @par Callers
 * func_80076A2C only, once per frame after the screen state has settled.
 */
s32 gFindModuleMovie(void **arg0)
{
    register void **arg0_reg __asm__("$20") = arg0;
    register s32 res __asm__("$19");
    register u32 len __asm__("$16");
    u32 i;
    char buffer[24];
    u8 c;
    void *s2;
    Entry *entry;

    len = gBiosStrlen(gFrontEndModuleName);
    i = 0;
    res = 0;
    if (len != 0) {
        do {
            c = gBiosToLower(gFrontEndModuleName[i]);
            buffer[i] = c;
            if (c == 0x2E) {
                break;
            }
            i++;
        } while (i < len);
    }
    buffer[i] = 0;

    if (gBiosStrcmp(buffer, gLax2ModuleName) == 0) {
        __builtin_memcpy((u32 *)buffer, gLax1ModuleName, 5);
    }

    i = 0;
    s2 = gCreditsMovieFilename;
    for (; i < 16; i++) {
        entry = &gWideTextTable[i];
        if (gBiosStrcmp(buffer, entry->unk0) == 0) {
            res = entry->unk8;
            if (arg0_reg == 0) {
                return res;
            }
            if (entry->unk21 != 0) {
                *arg0_reg = s2;
            } else {
                *arg0_reg = 0;
            }
            break;
        }
    }

    return res;
}
