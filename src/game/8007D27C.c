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
void D_8007D27C(s32 arg0)
{

    if (arg0 < 3) if (arg0 > 0) {
        func_80046C84(0x47, 0);
        func_800451B4(func_800451E0() ^ 1);
        if (func_800451E0()) {
            D_8009BABE[0] = 0x100E;
        } else {
            D_8009BABE[0] = 0x1018;
        }
    }
}
