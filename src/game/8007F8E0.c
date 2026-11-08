#include "game/types.h"

extern u8 *D_8009F92C;
extern u8 D_8009F8BD;
extern u8 D_8009F8BE;
extern u8 D_800A36BD;
extern void *D_8009FA94;

extern void func_8007FA88(void);
extern void func_800776B8(void);
extern void func_8007F02C(void);

void func_8007F8E0(void)
{
    D_8009F92C[0x921] = 0;
    D_8009F8BD = 0;
    D_8009F8BE = 0;
}
