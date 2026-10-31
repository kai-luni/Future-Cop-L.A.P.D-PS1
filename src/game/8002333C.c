#include "game/types.h"

typedef struct {
    s8 unk00;
    s8 unk01;
    u8 unk02;
    u8 pad03;
    s32 unk04;
    s32 unk08;
    u8 pad0C[4];
} Unk800A0A00;

extern Unk800A0A00 D_800A0A00[8];
extern s32 D_8009F640[2];
extern s32 D_800A0A80[3];
extern s32 D_8009F648;
extern s32 D_8009F278;
extern s32 D_8009F27C;
extern u8 D_8009F26C;
extern u8 D_800A8E48[];
extern u8 D_800AB0E8[];
extern u8 D_80023314[];

extern void func_80023478(void);
extern void func_80021B50(void);
extern s32 func_80021C20(s32 first, ...);
extern void func_800211FC(void);
extern s32 func_8001F0FC(s32 callback);
extern s32 func_80098458(s32 arg0, s32 arg1);
extern s32 func_80097B28(s32 arg0);
extern void func_80044F8C(void);

u8 func_8002333C(void)
{
    s32 result;
    u32 i;

    func_80023478();
    func_80021B50();
    result = func_80021C20(1, 0x1002001, 4, 8, 5, 0x18, 8, D_800A8E48, 9, D_800AB0E8, 0);
    func_800211FC();
    func_8001F0FC((s32)D_80023314);
    for (i = 0; i < 8; i++) {
        D_800A0A00[i].unk00 = -1;
        D_800A0A00[i].unk01 = -1;
        D_800A0A00[i].unk04 = 0;
        D_800A0A00[i].unk08 = 0;
        D_800A0A00[i].unk02 = 0;
    }
    i = 0;
    {
        register s32 *p __asm__("$3");
        p = D_8009F640;
        do {
            *p = 0;
            p++;
            i++;
        } while (i < 2);
    }
    if (result == 0) {
        D_8009F27C = func_80098458(0x1010, 0x11E80);
        D_8009F278 = func_80097B28(0xC000);
        D_8009F26C = 1;
    }
    D_800A0A80[2] = 0;
    D_800A0A80[0] = 0;
    D_800A0A80[1] = 0;
    D_8009F648 = 0;
    func_80044F8C();
    return D_8009F26C;
}
