#include "game/types.h"

typedef struct {
    u8 unk00[0x3C];
    void (*unk3C)(s32);
} Unk8009DDE8;

extern u8 D_8009DDF2;
extern void (*D_8009DDEC)(void *arg0, s32 arg1);
extern Unk8009DDE8 *D_8009DDE8;
extern u8 D_80012A44[];

void func_80092C8C(s32 arg0)
{
    if (D_8009DDF2 >= 2) {
        D_8009DDEC(D_80012A44, arg0);
    }

    D_8009DDE8->unk3C(arg0);
}
