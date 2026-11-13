#include "game/types.h"

typedef struct {
    u8 pad00[0xC];
    s32 unk0C;
    u8 pad10[0x4C];
    s32 unk5C;
} Unk8009FA94;

extern u8 D_8009F904;
extern s32 D_8009F908;
extern s32 D_8009F90C;
extern s32 D_8009F910;
extern volatile s32 D_8009F914;
extern s32 D_8009F918;
extern volatile s32 D_8009F91C;
extern volatile s32 D_8009F920;
extern Unk8009FA94 *D_8009FA94[2];
extern u16 D_8009BABE[];
extern u8 D_800A377B[];
extern void func_8002A16C(s32);
extern s32 func_8002A58C(void);
extern s32 func_800451E0(void);
extern s32 func_8004522C(void);
extern s32 func_80045288(void);
extern s32 func_800452C8(void);

void func_8007D18C(void)
{
    register Unk8009FA94 *p __asm__("$3");
    s32 tmp;

    D_8009F918 = 0;
    D_8009F908 = func_8004522C();
    D_8009F90C = func_80045288();
    D_8009F910 = func_800452C8();
    tmp = func_800451E0();
    D_8009F914 = tmp;
    D_8009F91C = 0;
    D_8009F920 = 0x47;
    D_8009BABE[0] = (tmp != 0) ? 0x100E : 0x1018;
    D_8009F904 = 0;
    if ((u8)func_8002A58C() == 0) {
        func_8002A16C(0);
    }
    __asm__ volatile("lui %0,%%hi(D_8009FA94)\n\tlw %0,%%lo(D_8009FA94)(%0)" : "=r" (p) : : "memory");
    if (p->unk5C == 1) {
        if (p->unk0C == 1) {
            D_800A377B[0] = 9;
        } else {
            D_800A377B[0] = 3;
        }
    } else if (p->unk0C == 1) {
        D_800A377B[0] = 1;
    } else {
        D_800A377B[0] = 8;
    }
}
