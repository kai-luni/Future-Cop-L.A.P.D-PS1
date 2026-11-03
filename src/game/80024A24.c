#include "game/types.h"

struct Obj {
    char pad0[0x0C];
    s32 unk0C;
    char pad1[0x18];
    s32 unk28;
};

struct Name {
    char b[10];
};

extern s32 D_8009F284;
extern s32 D_8009F668;
extern s32 D_8009F66C;
extern u8 D_8009F680;
extern s32 D_8009F684;
extern s32 D_8009F688;
extern s32 D_8009F68C;
extern s32 D_8009FA24;
extern s32 D_8009FA6C;
extern s32 D_8009FA70;
extern s32 D_8009FA84;
extern struct Obj *D_8009FA94;
extern s32 D_8009FAC0;

extern s32 D_8009F678[];
extern s32 D_8009FA78[];
extern struct Name D_80010DC0;
extern void *D_8009F9A0;
extern u8 D_800A0B30[];
extern u8 D_800A8540[];
extern s32 D_8009FA5C[];

extern void func_80017C68(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5);
extern void func_800860CC(void);
extern void func_8008CCB0(void *a0, s32 a1);
extern void func_80019D60(void);
extern void func_8006A880(void);
extern void func_80019A64(void);
extern void func_800197A8(s32 seed);
extern void func_80056924(void);
extern void func_80019828(void);
extern void func_8005A044(void);
extern void func_800609F4(void);
extern void func_80069B58(void);
extern void func_8001774C(void);
extern void func_8001B2C8(s32 a0, s32 a1);
extern void func_8004CAE4(void);
extern void func_8002A598(void);
extern void func_80049400(void);
extern void func_80060E28(void);
extern void func_8005130C(void);
extern void func_80013A14(void);
extern s32 func_80063428(void);
extern void func_80012E1C(s32 arg0);
extern void func_800617D8(void);
extern void func_8004783C(void);
extern void func_80059BFC(s32 a0, s32 a1, s32 a2);
extern void func_80029F50(void *a0);
extern void func_8005A738(void);
extern void func_80049690(void);
extern void func_8005A2B0(void);
extern void func_8008347C(void);
extern void func_80031984(void);
extern void func_8004AB4C(void);
extern void func_8003F79C(s32 a0);
extern void func_800734B0(void);

void func_80024A24(s32 arg0)
{
    struct Name name;
    s32 i;
    s32 v;
    s32 seed;
    s32 target;

    D_8009F68C = 0;
    D_8009F680 = 0;
    {
        register s32 *p __asm__("$4");
        register s32 *q __asm__("$5");

        i = 0;
        q = D_8009FA78;
        p = D_8009F678;
        do {
            *p = 0;
            *q = 0;
            q++;
            i++;
            p++;
        } while (i < 2);
    }

    name = D_80010DC0;

    D_8009FAC0 = 0;
    target = D_8009FA94->unk28;
    for (i = 0; i < 10; i += 2) {
        if (name.b[i] == target) {
            D_8009FAC0 = name.b[i + 1];
            goto call_init;
        }
    }

call_init:
    D_8009F9A0 = 0;
    D_8009F684 = arg0;
    func_80017C68(0x168, 0x82, 0x118, 0x64, 0xFC, 0x145);
    D_8009FA24 = 0;
    D_8009FA6C = 0;
    func_800860CC();
    func_8008CCB0(D_800A0B30, arg0);

    {
        register s32 n __asm__("$3");
        register u32 bp __asm__("$2");

        n = 0x7F;
        bp = (u32)D_800A8540;
        bp += 0x7F;
        do {
            *(u8 *)bp = 0;
            n--;
            bp -= 1;
        } while (n >= 0);
    }

    D_8009F668 = 0;
    D_8009F66C = 0;

    {
        s32 n = 1;
        s32 *bp = D_8009FA5C;

        do {
            *bp = 0;
            bp--;
            n--;
        } while (n >= 0);
    }

    func_80019D60();
    func_8006A880();
    func_80019A64();

    seed = D_8009F284;
    v = D_8009FA84;
    if (v != 0) {
        if (v > 0) {
            func_800197A8(seed);
            func_80056924();
            v = D_8009FA84;
        }
        seed = ((v >> 31) + v) ^ (v >> 31);
        D_8009F688 = seed;
    }

    func_80019828();
    func_800197A8(seed);
    func_8005A044();
    func_800609F4();
    func_80069B58();
    func_8001774C();
    func_8001B2C8(0x898, 0x400);
    func_8004CAE4();
    func_8002A598();
    func_80049400();
    func_80060E28();
    func_8005130C();
    func_80013A14();
    func_80012E1C(func_80063428());
    func_800617D8();
    func_8004783C();

    if (D_8009FA84 != 0) {
        s32 flag = (D_8009FA94->unk0C == 2) ? 2 : 1;

        func_80059BFC(flag, D_8009F688, arg0);
    }

    func_80029F50(D_800A0B30);
    D_8009FA70 = 0;
    func_8005A738();
    func_80049690();
    func_8005A2B0();
    func_8008347C();
    func_80031984();
    func_8004AB4C();
    func_8003F79C(arg0);
    func_800734B0();
}
