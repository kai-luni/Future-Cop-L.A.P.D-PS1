#include "game/types.h"

typedef struct {
    u8 pad00[0x8];
    s32 unk8;
    s32 unkC;
    s32 unk10;
    u8 pad14[0x14];
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    u8 pad40[0xC];
    s32 unk4C;
    u8 pad50[0xC];
    s32 unk5C;
    u8 pad60[0x2C];
    s32 unk8C;
    s32 unk90;
    s32 unk94;
    s32 unk98;
} State;

extern State *D_8009FA94;
extern s32 D_8009FA2C;
extern s32 D_8009F9A8;
extern s32 D_8009F850;
extern s32 D_8009F854;
extern s32 D_8009F858;
extern s32 D_8009F85C;
extern s32 D_8009F860;
extern s32 D_8009F864;
extern s8 D_8009F868;
extern s8 D_8009F869;
extern u8 D_8009F3E5;
extern u8 D_8009F3F0;
extern u8 D_8009F4E0;
extern s32 D_8009F3EC;
extern s32 D_8009F3D0;
extern u8 D_8009F3A0[];
extern u8 D_8009F3A8[];
extern u8 D_8009F3B0[];
extern u8 D_8009B300[];
extern u8 D_80011E3C[];
extern u8 D_80011E48[];

extern void func_800965C8(void);
extern u8 func_80076EB0(void);
extern void func_80076FE0(void);
extern void func_8007BFCC(s32, s32);
extern void func_80077050(void);
extern void func_8001B2C8(s32, s32);
extern void func_80025370(void);
extern void func_8007F9CC(void);
extern void func_8007F8BC(void);
extern void func_8007FA88(void);
extern void func_8007F8E0(void);
extern void func_80080B08(void);
extern s32 func_8007F8CC(void);
extern void func_8007FACC(void);
extern void func_8007FA48(void);
extern void func_80089D44(void *, s32, s32, s32, s32, s32);
extern void func_8008CD30(void *, void *, s32, s32);
extern void func_8001B448(void);
extern void func_80077088(void);
extern void func_800780C0(s32);
extern void func_800287F4(void *, s32);
extern void func_8008CC90(void *, void *);
extern void func_8008CCB0(void *, s32);
extern s32 func_80083130(void *);

s32 func_80076A2C(void)
{
    u8 local[0x54];
    s32 s0;
    s32 s3;
    u8 s2;

    func_800965C8();
    s2 = 1;
    if (func_80076EB0() != 0) {
        register s32 z0 __asm__("$4") = 0;
        register s32 z1 __asm__("$5") = z0;
        register s32 ten __asm__("$6");
        __asm__ volatile("" : : "r"(z0), "r"(z1));
        ten = 0x10;
        D_8009FA94->unk30 = 0x20;
        D_8009FA94->unk3C = 0x20;
        D_8009FA94->unk28 = 0;
        D_8009FA94->unk2C = ten;
        D_8009FA94->unk34 = 0;
        D_8009FA94->unk38 = ten;
        D_8009FA94->unk8C = 0;
        D_8009FA94->unk90 = 0;
        D_8009FA94->unk94 = 0;
        D_8009FA94->unk8 = 1;
        D_8009FA94->unk98 = 1;
        D_8009FA94->unk4C = 0;

        func_8007BFCC(z0, z1);
        func_8007BFCC(1, 0);

        D_8009F9A8 = 3;
        D_8009FA94->unk10 = 3;
        func_80077050();

        func_8001B2C8(0xC8, 0x200);

        D_8009F3E5 = 0;
        D_8009F3F0 = 0x14;

        func_80025370();
        func_8007F9CC();
        func_8007F8BC();
        func_8007FA88();
        func_8007F8E0();

        s0 = D_8009FA2C + 0x5DC;
        while (D_8009FA2C < s0 || func_8007F8CC() == 0) {
            func_80080B08();
        }

        s0 = 0x140;
        func_8007FACC();
        s3 = 1;
        func_8007FA48();

        func_80089D44(D_80011E3C, 4, 0, 0, s0, 0xF0);

        func_8008CD30(local, D_8009F3A0, D_8009F3D0, (s32)D_80011E48);
        func_80089D44(local, 4, 0, 0x18, s0, 0xC0);

        func_8001B448();
        func_80077088();
        func_800780C0(0);

        for (;;) {
            if (s2) {
                func_800287F4(D_8009F3A8, 1);
                func_8008CC90(D_8009B300, D_8009F3B0);
                s2 = 0;
                continue;
            }

            D_8009F850 = D_8009FA94->unk28;
            D_8009F854 = D_8009FA94->unk2C;
            D_8009F858 = D_8009FA94->unk30;
            D_8009F85C = D_8009FA94->unk34;
            D_8009F860 = D_8009FA94->unk38;
            D_8009F864 = D_8009FA94->unk3C;
            D_8009F868 = ((u8 *)D_8009FA94)[8];
            D_8009F869 = ((u8 *)D_8009FA94)[0x98];

            func_800287F4(D_8009B300, 0);

            D_8009FA94->unk28 = D_8009F850;
            D_8009FA94->unk2C = D_8009F854;
            D_8009FA94->unk30 = D_8009F858;
            D_8009FA94->unk34 = D_8009F85C;
            D_8009FA94->unk38 = D_8009F860;
            D_8009FA94->unk3C = D_8009F864;
            ((s32 *)D_8009FA94)[2] = D_8009F868;
            ((s32 *)D_8009FA94)[0x26] = D_8009F869;

            if (D_8009FA94->unk4C == 10 &&
                D_8009FA94->unk5C == s3 &&
                D_8009FA94->unkC == s3) {
                D_8009FA94->unk10 = 0;
            }

            D_8009F9A8 = D_8009FA94->unk10;
            switch (D_8009FA94->unk10) {
            case 0:
                if (D_8009F3EC != 0) {
                    func_8008CCB0(D_8009B300, D_8009F3EC);
                    func_8008CC90(D_8009B300, D_8009F3B0);
                    D_8009F9A8 = 3;
                    D_8009F3EC = 0;
                    D_8009F3F0 = 5;
                    break;
                }
            case 3:
                D_8009F3F0 = s3;
                s2 = 1;
            }

            if (D_8009F9A8 != 0) {
                continue;
            }

            s0 = func_80083130(local + 0x50);
            if (s0 == 0 && *(s32 *)(local + 0x50) == 0) {
                continue;
            }

            func_80077050();

            if (s0 != 0) {
                func_8008CD30(local + 0x28, D_8009F3A0, D_8009F3D0, s0);
                func_80089D44(local + 0x28, 4, 0, 0x18, 0x140, 0xC0);
            }

            if (D_8009F4E0 == 0 && *(s32 *)(local + 0x50) != 0) {
                func_8008CD30(local + 0x28, D_8009F3A0, D_8009F3D0, *(s32 *)(local + 0x50));
                func_80089D44(local + 0x28, 4, 0, 0x18, 0x140, 0xC0);
            }

            func_80077088();
        }
    }

    func_80076FE0();
    return 0;
}
