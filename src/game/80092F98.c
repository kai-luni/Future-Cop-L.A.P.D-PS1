#include "game/types.h"

typedef struct {
    u8 unk00[0x8];
    void (*unk08)(s32, s32, s32, s32);
    u8 unk0C[0x10];
    s32 unk1C;
} Unk8009DDE8;

extern u8 D_80012AA0[];
extern Unk8009DDE8 *D_8009DDE8;

extern void func_80092CF4(u8 *arg0, s32 arg1);

void func_80092F98(s32 arg0, s32 arg1)
{
    func_80092CF4(D_80012AA0, arg0);
    D_8009DDE8->unk08(D_8009DDE8->unk1C, arg0, 8, arg1);
}
