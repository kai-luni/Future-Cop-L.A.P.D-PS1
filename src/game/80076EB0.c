#include "game/types.h"

extern u32 D_8009EA04;
extern u32 D_8009EA08;
extern u32 *D_8009FA94;
extern u32 *D_8009FAD4;
extern u32 *D_8009F9A4;
extern u32 *D_8009F9F8;
extern u8 D_8009B2DC[];

extern void func_8008CA30(u32 arg0, u32 arg1);
extern u32 *func_8001E240(s32 arg0, s32 arg1);
extern void func_80017BE8(void);
extern void func_8008DEA0(void);
extern s32 func_8001E4E4(void);
extern s32 func_8001BF78(void);
extern void func_8001C088(s32 mode, s32 enable);
extern void func_8001C040(u32 color);
extern s32 func_8001BFE4(void);
extern void func_8001D248(s32 arg0, s32 arg1, u8 *arg2);
extern void func_8001F0B8(void);
extern void func_8002333C(void);

s32 func_80076EB0(void)
{
    register s32 result __asm__("$16");
    s32 i;

    result = 0;
    func_8008CA30(D_8009EA04, (D_8009EA08 >> 3) << 3);

    D_8009FA94 = func_8001E240(0xF4, 3);
    for (i = 0x38; i >= 0; i--) {
        D_8009FA94[i] = 0;
    }

    D_8009FAD4 = func_8001E240(0x190, 3);
    D_8009F9A4 = func_8001E240(0x80, 3);
    D_8009F9F8 = func_8001E240(0x80, 3);

    func_80017BE8();
    func_8008DEA0();

    if (func_8001E4E4() != 0) {
        goto done;
    }
    if (func_8001BF78() != 0) {
        goto done;
    }

    func_8001C088(1, 1);
    func_8001C040(0);
    func_8001C088(0, 1);
    func_8001C040(0);

    result = 1;
    func_8001BFE4();
    func_8001D248(0, 0, D_8009B2DC);
    func_8001F0B8();
    func_8002333C();

done:
    return result;
}
