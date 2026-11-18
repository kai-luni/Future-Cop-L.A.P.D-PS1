// Categories: Game Engine > Front-End Configuration | Resources > Text Tables
#include "game/types.h"

struct Name7 {
    s8 c[7];
};

extern struct Name7 gPlayerNameTable __asm__("D_800A6FE0");
extern struct Name7 D_800A6FE9;
extern struct Name7 D_8009F474;
extern struct Name7 D_8009F47C;
extern u32 gTitleScreenDeadline __asm__("D_8009F8E8");
extern u8 gSetupDoneFlag __asm__("D_8009F3E4");

extern u8 *func_80089914(s32 id);
extern s32 func_8008CCC0(u8 *s);
extern void func_8008CCB0(void *dst, u8 *src);

/**
 * @brief One-time front-end name setup: resolves text ids 0xC8/0xD2 into the two player name buffers with default fallbacks, and arms a deadline word.
 *
 * @param void
 *
 * Entered through func_80077990, a 12-byte prologue fragment that loads the tick counter D_8009FA2C into $v0 and the setup flag D_8009F3E4 into $v1, then falls through; the C models these as uninitialised registers. The listing marks this unit nonmatching.
 *
 * @par Globals
 * D_8009F8E8 - deadline, set to D_8009FA2C + 0x5DC. D_8009F3E4 - setup flag; the body runs only while 0 and sets it to 1 only when both names resolved. D_800A6FE0/D_800A6FE9 - the two name buffers (9-byte stride, drawn by the name screens). D_8009F474/D_8009F47C - default name strings copied on fallback.
 *
 * @par Control flow
 * Per name: resolve the text id; if a string exists and strlen < 9, strcpy it into the buffer, otherwise copy the default and mark the fallback. Only when neither fallback ran is D_8009F3E4 set.
 *
 * @par Calls
 * func_80089914 - text-table binary search for ids 0xC8 and 0xD2. func_8008CCC0 - BIOS strlen. func_8008CCB0 - BIOS strcpy.
 *
 * @par Side effects
 * Writes the deadline word, both name buffers and the setup flag.
 *
 * @par Callers
 * Confirmed: func_80077AD4 installs func_80077990 as the unk10 init callback of menu state D_800A38F8 (address taken at 0x80077B40); a direct call to the fragment sits at 0x80078F94 in a non-catalog routine. Unknown: the remaining package callers have no call edges.
 */
void func_8007799C(void)
{
    register u32 base __asm__("$2");
    register s32 flag __asm__("$3");
    u8 *p;
    u8 ok;

    gTitleScreenDeadline = base + 0x5DC;
    ok = 0;
    if (flag == 0) {
        p = func_80089914(0xC8);
        if (p != 0 && func_8008CCC0(p) < 9) {
            func_8008CCB0(&gPlayerNameTable, p);
        } else {
            gPlayerNameTable = D_8009F474;
            ok = 1;
        }
        p = func_80089914(0xD2);
        if (p != 0 && func_8008CCC0(p) < 9) {
            func_8008CCB0(&D_800A6FE9, p);
        } else {
            D_800A6FE9 = D_8009F47C;
            ok = 1;
        }
        if (ok == 0) {
            gSetupDoneFlag = 1;
        }
    }
}
