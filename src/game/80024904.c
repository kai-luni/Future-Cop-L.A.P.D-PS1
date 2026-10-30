#include "game/types.h"

extern s32 D_8009F284;
extern s32 D_8009F668;
extern s32 D_8009F66C;
extern s32 D_8009F684;
extern s32 D_8009FA70;
extern u8 D_800A0B30[];

extern void func_80017C68(s32, s32, s32, s32, s32, s32);
extern void func_800197A8(s32 seed);
extern void func_80019828(void);
extern void func_80019A64(void);
extern void func_8001B2C8(s32, s32);
extern void func_80029F50(void *);
extern void func_8004783C(void);
extern void func_80069B58(void);
extern void func_800780DC(void);
extern void func_8008CCB0(void *, s32);

void func_80024904(s32 arg)
{
    s32 temp;

    D_8009F684 = arg;
    func_80017C68(0x168, 0, 0x118, 0, 0xFC, 0);
    func_8008CCB0(D_800A0B30, arg);
    D_8009F668 = 0;
    D_8009F66C = 0;
    func_80019A64();
    temp = D_8009F284;
    func_80019828();
    func_800197A8(temp);
    func_80069B58();
    func_8001B2C8(0xC8, 0x200);
    func_8004783C();
    func_80029F50(D_800A0B30);
    D_8009FA70 = 0;
    func_800780DC();
}
