// Categories: Resources > Text Tables | Graphics > Text
#include "game/types.h"

typedef struct Unk800899A4 {
    u16 *unk0;
    u8 unk04[0x18];
    s32 unk1C;
} Unk800899A4;

extern Unk800899A4 *gTextLookupTable __asm__("D_8009F518");
extern void gFreeBlock(Unk800899A4 *arg0) __asm__("func_800181D4");
void gLoadTextLookupTable(Unk800899A4 *arg0) __asm__("func_800899A4");


/**
 * @brief Registers and validates the loaded binary-search text table resource.
 *
 * Rejects resources whose unk1C version/mode word is not 1; otherwise frees any
 * prior table in D_8009F518, installs arg0, and frees/clears it if unk0[0] != 1.
 *
 * @param arg0 Loaded text resource wrapper with header pointer at +0x00 and unk1C at +0x1C.
 *
 * @par Globals
 * D_8009F518 - global pointer to the active text lookup table (read by func_80089914).
 *
 * @par Control flow
 * Frees arg0 and returns early if unk1C != 1; frees old D_8009F518 if non-null, stores arg0,
 * sets validation flag if arg0->unk0[0] != unk1C, and frees/clears D_8009F518 on failure.
 *
 * @par Calls
 * func_800181D4 - frees rejected arg0 or previously active D_8009F518.
 *
 * @par Side effects
 * Updates D_8009F518 and may free heap memory via func_800181D4.
 *
 * @par Callers
 * Confirmed: func_80024EC8 (asset loader dispatch on tag 0x43747273 / "Ctrs").
 */
void gLoadTextLookupTable(Unk800899A4 *arg0)
{
    s32 temp = arg0->unk1C;
    s32 flag = 0;
    s32 id;

    if (temp != 1) {
        gFreeBlock(arg0);
        return;
    }
    if (gTextLookupTable != 0) {
        gFreeBlock(gTextLookupTable);
    }
    id = arg0->unk0[0];
    gTextLookupTable = arg0;
    if (id != temp) {
        flag = 1;
    }
    if (flag != 0) {
        gFreeBlock(arg0);
        gTextLookupTable = 0;
    }
}
