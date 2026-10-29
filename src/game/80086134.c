#include "game/types.h"

/* gp-relative small data: offsets 0x734 and 0x738 from $gp */
extern s32 D_8009F948;
extern s32 D_8009F94C;

extern void func_80089A88(s32 arg0, s32 arg1, s32 arg2);
extern void func_80085F1C(void);

void func_80086134(void)
{
    func_80089A88(D_8009F948, D_8009F94C, 0);
    func_80085F1C();
}
