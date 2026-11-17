// Categories: Graphics > Text/Font | Resources > Asset Selection
#include "game/types.h"

struct FontEntry {
    u8 data[0x11C];
};

extern struct FontEntry D_800A19A0[];
extern s32 D_8009F364;
extern s32 D_8009F76C;
extern struct FontEntry *gFont __asm__("D_8009F35C");

extern void func_80048DEC(void);

/**
 * @brief Selects one of the loaded font/character banks.
 *
 * @param index Bank index; anything >= D_8009F364 is rejected.
 *
 * @return 1 when the bank was selected, 0 when the index was out of range.
 *
 * @par Confirmed flow
 * D_8009F76C = index, D_8009F35C = &D_800A19A0[index] (entries of 0x11C bytes)
 * and func_80048DEC() re-binds the active font. The rejection path writes
 * nothing at all, so the previous selection stays active.
 *
 * @par Globals
 * D_8009F364 (bank count), D_8009F76C (current index), D_8009F35C (current
 * bank pointer), D_800A19A0 (bank array).
 *
 * @par Callers
 * func_800287F4 - called with func_80048E94(), which simply returns 0, so the
 * front-end always selects bank 0. Other callers: func_80048A14,
 * func_800788F0, func_8007E1A4 and the unit at 0x800788BC.
 */
s32 func_80048E28(s32 index)
{
    s32 result;

    if (index < D_8009F364) {
        goto valid;
    }
    result = 0;
    goto done;
valid:
    D_8009F76C = index;
    gFont = &D_800A19A0[index];
    func_80048DEC();
    result = 1;
done:
    return result;
}
