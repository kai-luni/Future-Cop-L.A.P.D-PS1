#include "game/types.h"

struct UnkFA94 {
    u8 pad0[0xC];
    s32 unkC;
};

extern u8 *D_8009F92C;
extern u8 D_8009F8BD;
extern u8 D_8009F8BE;
extern u8 D_800A36BD[];
extern struct UnkFA94 *D_8009FA94;

extern void func_8007FA88(void);
extern void func_8007F02C(void);

void func_8007F93C(void)
{
    u8 *p;

    D_8009F92C[0x93D] = 0;
    D_8009F92C[0x93F] = 0;
    D_8009F92C[0x940] = 1;
    func_8007FA88();
    D_8009F92C[0x921] = 1;
    p = D_8009F92C;
    D_8009F8BD = 0;
    D_8009F8BE = 0;
    *(u16 *)(p + 0x942) = 0;
    *(u8 *)(p + 0x944) = 0;
    func_8007F02C();
    if (D_8009FA94->unkC == 2) {
        D_800A36BD[0] = 0;
    } else {
        D_800A36BD[0] = 1;
    }
}
