#include "game/types.h"

extern s32 D_8009F9C4;
extern s32 D_8009FA0C;
extern u8 D_8009FA3C;
extern s32 D_8009FAA4;
extern u8 D_8009FAA8;

extern u8 func_80024E3C(s32 arg0);
extern void func_800259D8(void);
extern void func_80029E64(void);
extern void func_80010030(s32 arg0, s32 arg1, s32 arg2);
extern s32 func_80048E94(void);
extern s32 func_80048E28(s32 arg0);
extern void func_8003FAD4(void);
extern void func_80027EE4(void);
extern void func_80028618(s32 arg0);
extern void func_80025B7C(s32 arg0);

s32 func_800287F4(s32 arg0, u8 arg1)
{
    D_8009FA0C = 0;
    D_8009F9C4 = arg1;
    D_8009FAA4 = 0;
    D_8009FAA8 = 0;

    if (func_80024E3C(arg0) == 0) {
        return 0;
    }

    D_8009FA3C = 0;
    func_800259D8();
    func_80029E64();

    func_80010030((D_8009FA0C == 1) ? 0xA0 : 0x50, 0x78, 0x200);
    func_80048E28(func_80048E94());
    func_8003FAD4();

    if (D_8009F9C4 != 0) {
        func_80027EE4();
    } else {
        func_80028618(arg0);
    }

    func_80025B7C(arg0);
    return 0;
}
