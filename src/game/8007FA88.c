#include "game/types.h"

typedef struct {
    u8 pad0[0x200];
    u8 unk200;
    u8 pad201[2];
    u8 unk203;
    u8 pad204[4];
    s32 unk208;
} UnkStruct;

extern UnkStruct *D_8009F92C;
extern u8 D_8009F496;

extern void func_800206D0(void);

void func_8007FA88(void)
{
    register s32 six __asm__("$5");

    func_800206D0();

    {
        register UnkStruct *p __asm__("$4") = D_8009F92C;
        six = 6;
        p->unk200 = six;
        p->unk208 = -0x2B;
    }
    {
        register UnkStruct *q __asm__("$3") = D_8009F92C;
        register s32 one __asm__("$2") = 1;
        q->unk203 = six;
        D_8009F496 = one;
    }
}
