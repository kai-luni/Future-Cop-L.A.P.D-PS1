// Categories: Resources > Asset Registry
#include "game/types.h"

struct Unk800A0B70 {
    u8 unk00[0x2C];
    void *unk2C;
};

extern struct Unk800A0B70 D_800A0B70;
extern s32 D_8009F6E8;
extern void func_80019584();

/**
 * @brief Releases the asset registered in D_800A0B70 if nothing else is using
 *        it.
 *
 * When D_800A0B70.unk2C (the registered pointer) is non-NULL and D_8009F6E8 is
 * zero, the pointer is passed to func_80019584() - the pool/object destructor -
 * and the slot is cleared.
 *
 * @return Nothing.
 *
 * @par Globals
 * D_800A0B70 (a 0x30-byte descriptor whose last field is the pointer),
 * D_8009F6E8 ("in use" counter/flag).
 *
 * @par Callers
 * func_800287F4, right after the module load and before the frame work.
 */
void func_80029E64(void)
{
    if (D_800A0B70.unk2C != 0 && D_8009F6E8 == 0) {
        func_80019584(D_800A0B70.unk2C);
        D_800A0B70.unk2C = 0;
    }
}
