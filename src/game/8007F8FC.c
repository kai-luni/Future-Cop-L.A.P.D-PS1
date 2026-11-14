#include "game/types.h"

extern u8 D_8009F8BD;
extern u8 D_8009F8BE;
extern u8 *D_8009F92C;

extern void func_8007FA88(void);
extern void func_8007F8E0(void);
extern void func_800776B8(void);

void func_8007F8FC(void)
{
    u8 *p;

    func_8007FA88();
    func_8007F8E0();
    p = D_8009F92C;
    D_8009F8BD = 0;
    D_8009F8BE = 0;
    *(u16 *)(p + 0x942) = 0;
    *(u8 *)(p + 0x944) = 0;
    func_800776B8();
}
