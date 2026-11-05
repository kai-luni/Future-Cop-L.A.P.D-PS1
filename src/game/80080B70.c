#include "game/types.h"

typedef struct {
    u8 pad0[0x1F];
    u8 unk1F;
    u8 unk20;
    u8 pad21[3];
} Entry;

extern u8 *D_8009F92C;
extern u8 *D_8009FA94;
extern u8 D_800A6FE0[];
extern Entry D_8009B4E4[];
extern s8 D_8009B72C[];
extern u8 D_800120A4[];
extern u8 D_8009F3C0;
extern u8 D_8009F3C1;
extern u8 D_8009F3C3;
extern u8 D_8009F3C4;
extern u8 D_8009F3E6;
extern u8 D_8009F3E7;
extern u8 D_8009F458;
extern u8 D_8009F3F1;
extern u8 D_8009FAC4;
extern u8 D_8009F8BD;
extern u8 D_8009F8BE;

extern u8 *func_80089914(u16 id);
extern void func_800822CC(u8 *arg0);
extern u8 *func_8001E318(u8 *destination, u8 *source, u32 count);
extern void func_80080A5C(u8 *arg0);
extern void func_8008CCB0(u8 *arg0, u8 *arg1);
extern void func_8007F6E0(void);
extern s32 func_80020FDC(u8 *arg0, u8 *arg1, s32 arg2, s32 arg3);

typedef struct { u8 b[0x70]; } Rec;
typedef struct { u8 pad0[0x424]; Rec recs[10]; u8 pad884[0xA0]; u8 idx; } Big;
#define S(off) (((Big *)D_8009F92C)->recs[((Big *)D_8009F92C)->idx].b[(off) - 0x424])
#define REC (((Rec *)((Big *)D_8009F92C)->recs)[((Big *)D_8009F92C)->idx].b - 0x424)

void func_80080B70(void)
{
    u8 *r;
    u32 i;

    r = func_80089914(0x36C4);
    if (r != 0) {
        func_800822CC(r);
    }
    func_8001E318(D_8009F92C + 0x20C, D_8009F92C, 0x200);
    *(s32 *)(D_8009F92C + 0x40C) = 1;
    func_80080A5C(D_8009F92C + 0x40C);
    func_80080A5C(REC + 0x474);
    func_8008CCB0(REC + 0x440, D_800A6FE0);
    func_8008CCB0(REC + 0x44C, D_800A6FE0 + 9);
    S(0x459) = D_8009FA94[0x8C];
    S(0x45A) = D_8009FA94[0x90];
    S(0x45B) = D_8009FA94[0x94];
    S(0x45C) = D_8009FA94[0x28];
    S(0x45D) = D_8009FA94[0x34];
    S(0x45E) = D_8009FA94[0x2C];
    S(0x45F) = D_8009FA94[0x38];
    S(0x460) = D_8009FA94[0x30];
    S(0x461) = D_8009FA94[0x3C];
    S(0x462) = D_8009FA94[0x8];
    S(0x463) = D_8009FA94[0x98];
    S(0x470) = D_8009FA94[0xC];
    S(0x468) = D_8009FA94[0x5C];
    S(0x469) = D_8009F3C0;
    S(0x46A) = D_8009F3E6;
    S(0x46B) = D_8009F3E7;
    S(0x46C) = D_8009F3C1;
    S(0x46D) = D_8009F3C3;
    S(0x46F) = D_8009F3C4;
    S(0x46E) = D_8009B4E4[D_8009B72C[4]].unk1F;
    for (i = 0; i < 4; i++) {
        S(0x464 + i) = D_8009B4E4[D_8009B72C[i]].unk1F;
    }
    S(0x458) = D_8009F458;
    D_8009F3F1 = D_8009F458;
    func_8008CCB0(REC + 0x428, D_8009F92C + 0x925);
    func_8007F6E0();
    D_8009F92C[0x922] = 3;
    D_8009F92C[0x201] = 2;
    {
        register u8 *p __asm__("$2") = D_8009F92C;
        __asm__("" : "+r"(p));
        p[0x202] = 1;
        D_8009FAC4 = 1;
    }
    D_8009F8BD = 0;
    D_8009F8BE = 0;
    D_8009B4E4[D_8009B72C[0]].unk20 = D_8009B4E4[D_8009B72C[0]].unk1F;
    D_8009B4E4[D_8009B72C[1]].unk20 = D_8009B4E4[D_8009B72C[1]].unk1F;
    D_8009B4E4[D_8009B72C[2]].unk20 = D_8009B4E4[D_8009B72C[2]].unk1F;
    D_8009B4E4[D_8009B72C[3]].unk20 = D_8009B4E4[D_8009B72C[3]].unk1F;
    D_8009B4E4[D_8009B72C[4]].unk20 = D_8009B4E4[D_8009B72C[4]].unk1F;
    for (;;) {
        if (func_80020FDC(D_800120A4, D_8009F92C + 0x20C, 0x678, 0) != 0) {
            break;
        }
        if (*(s8 *)(D_8009F92C + 0x922) != 0) {
            D_8009F92C[0x922] = D_8009F92C[0x922] - 1;
            continue;
        }
        D_8009F92C[0x200] = 2;
        D_8009F92C[0x201] = 0;
        D_8009F92C[0x204] = 2;
        D_8009F92C[0x202] = 3;
        D_8009F8BD = 1;
        D_8009F8BE = 1;
        break;
    }
}
