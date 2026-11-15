// Categories: Game Engine > Options Menu | Graphics > Text > Formatting
#include "game/types.h"

struct Unk25F58 {
    u8 unk00[4];
    u8 *unk04;
    u8 *unk08;
    u8 *unk0C;
};

extern struct Unk25F58 *D_8009F2CC;
extern u8 D_8009F2EC[];
extern u8 D_8009ABE8[];

extern s32 func_800451E0(void);
extern s32 func_8004522C(void);
extern s32 func_80045288(void);
extern s32 func_800452C8(void);
extern s32 func_8008CCC0(u8 *arg0);
extern void func_8008CD30(u8 *arg0, u8 *arg1, s32 arg2);

/**
 * @brief Refreshes the three volume percentage labels and the stereo/mono
 *        marker of the options screen.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * For each of the three labels (D_8009F2CC->unk04/unk08/unk0C): value =
 * (getter() + 1) * 100 / 128 with func_8004522C/func_80045288/
 * func_800452C8, formatted by func_8008CD30 into the last three bytes
 * (ptr + strlen(ptr) - 3, strlen via the BIOS func_8008CCC0) using the
 * number template D_8009F2EC. D_8009ABE8[4] becomes 4 when func_800451E0
 * is non-zero, else 5.
 *
 * @par Likely role
 * The live update of the options page (the same three getters feed the
 * gauges of func_8007CFA0): the +1/128 scaling maps the 0..127 volume
 * range to 0..100 percent, the marker byte likely selects the stereo
 * (4) or mono (5) glyph.
 *
 * @par Unknown
 * What func_800451E0 reports (likely the stereo flag).
 *
 * @par Callers
 * func_800260B8, func_800267C8.
 */
void func_80025F58(void)
{
    s32 value;
    u8 *ptr;

    value = (func_8004522C() + 1) * 100 / 128;
    ptr = D_8009F2CC->unk04;
    func_8008CD30(ptr + func_8008CCC0(ptr) - 3, D_8009F2EC, value);

    value = (func_80045288() + 1) * 100 / 128;
    ptr = D_8009F2CC->unk08;
    func_8008CD30(ptr + func_8008CCC0(ptr) - 3, D_8009F2EC, value);

    value = (func_800452C8() + 1) * 100 / 128;
    ptr = D_8009F2CC->unk0C;
    func_8008CD30(ptr + func_8008CCC0(ptr) - 3, D_8009F2EC, value);

    value = func_800451E0();
    D_8009ABE8[4] = value ? 4 : 5;
}
