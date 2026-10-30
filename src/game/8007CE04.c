#include "game/types.h"

typedef struct {
    u8 pad00[8];
    u16 unk08;
    u8 pad0A[2];
    u8 unk0C;
    u8 pad0D[3];
    u16 unk10;
    u8 pad12[2];
    u8 unk14;
    u8 pad15[3];
    u16 unk18;
    u8 pad1A[2];
    u8 unk1C;
    u8 pad1D[3];
    u16 unk20;
    u8 pad22[2];
    u8 unk24;
} Unk8007CE04;

typedef struct {
    u8 pad00[0xC];
    s32 unk0C;
    u8 pad10[0x4C];
    s32 unk5C;
} Unk8009FA94;

extern s8 *D_8009F468;
extern u8 D_8009F904;
extern s32 D_8009F908;
extern s32 D_8009F90C;
extern s32 D_8009F910;
extern volatile s32 D_8009F914;
extern s32 D_8009F918;
extern volatile s32 D_8009F91C;
extern volatile s32 D_8009F920;

extern s32 D_8009FA60[];
extern Unk8009FA94 *D_8009FA94[2];
extern u16 D_8009BABE[];
extern u8 D_800A377B[];

extern void func_80082FA4(s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern Unk8007CE04 *func_80082AB0(s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_800788F0(s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80078B34(s32, s32, s32);
extern void func_8002A16C(s32);
extern s32 func_8002A58C(void);
extern void func_800451B4(s32 enable);
extern s32 func_800451E0(void);
extern s32 func_8004522C(void);
extern s32 func_80045288(void);
extern s32 func_800452C8(void);
extern void func_80046A34(s32 handle);
extern s32 func_80046C84(s32 id, s32 arg1);
void func_8007CE04(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    Unk8007CE04 *p;
    s32 diff;
    register s32 product __asm__("$2");
    register s32 quot __asm__("$7");

    func_80082FA4(0x9F, arg1 - 1, 0x5A, 6, 0x14, 0, 0, 0xCD, 2);
    p = func_80082AB0(arg0, 0xA0, arg1, -1, -1, 0x14, 0x80, 1, 8);
    if (p != 0) {
        diff = p->unk14 - p->unk0C;
        product = arg2 * diff;
        quot = product / arg3;
        if (arg2 != 0 && quot == 0) {
            quot = 1;
        }
        p->unk14 = p->unk0C + quot;
        p->unk24 = p->unk1C + quot;
        p->unk10 = p->unk08 + quot;
        p->unk20 = p->unk18 + quot;
        if (arg2 == 0) {
            func_800788F0(0, diff / 2 + 0xA0, arg1 - 2, 0xF, 0x10A4, 4, 8, 2);
        }
    }
}
