// Categories: Game Engine > Options Menu | Graphics > Text > Formatting
#include "game/types.h"

struct Unk25F58 {
    u8 unk00[4];
    u8 *unk04;
    u8 *unk08;
    u8 *unk0C;
};

extern struct Unk25F58 *gLabelRecord __asm__("D_8009F2CC");
extern u8 gOptionsVolumeFormat[] __asm__("D_8009F2EC");
extern u8 gItemCodeTable[] __asm__("D_8009ABE8");

extern s32 func_800451E0(void);
extern s32 func_8004522C(void);
extern s32 func_80045288(void);
extern s32 func_800452C8(void);
extern s32 func_8008CCC0(u8 *arg0);
extern void func_8008CD30(u8 *arg0, u8 *arg1, s32 arg2);

void gRefreshOptionsAudioLabels(void) __asm__("func_80025F58");

/**
 * @brief Refreshes three volume percentage fields and the stereo or mono selection code.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Computes (level + 1) times 100 divided by 128 for three getters, then formats each value at
 * the corresponding string end minus three using the %03ld template. Writes item code 4 when
 * the sound mode query is nonzero, otherwise 5.
 *
 * @par Globals
 * Reads label pointers in gLabelRecord and the gOptionsVolumeFormat template, writes labels and
 * gItemCodeTable[4].
 *
 * @par Callers
 * Direct calls from gDrawOptionsSubPanel (func_800260B8), gUpdateMapScreenView (func_800267C8).
 *
 * @par Role and path
 * The map and options screens refresh their audio labels and selection codes after settings
 * changes.
 *
 * @par Unknown
 * The item table's rendering consumer determines the glyphs for codes 4 and 5.
 */
void gRefreshOptionsAudioLabels(void)
{
    s32 value;
    u8 *ptr;

    value = (func_8004522C() + 1) * 100 / 128;
    ptr = gLabelRecord->unk04;
    func_8008CD30(ptr + func_8008CCC0(ptr) - 3, gOptionsVolumeFormat, value);

    value = (func_80045288() + 1) * 100 / 128;
    ptr = gLabelRecord->unk08;
    func_8008CD30(ptr + func_8008CCC0(ptr) - 3, gOptionsVolumeFormat, value);

    value = (func_800452C8() + 1) * 100 / 128;
    ptr = gLabelRecord->unk0C;
    func_8008CD30(ptr + func_8008CCC0(ptr) - 3, gOptionsVolumeFormat, value);

    value = func_800451E0();
    gItemCodeTable[4] = value ? 4 : 5;
}
