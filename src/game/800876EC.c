#include "game/types.h"

typedef struct {
    u8 pad0[0x8];
    volatile s32 unk8;
    u8 padC[0x7B58];
    volatile u32 unk7B64;
    volatile u32 unk7B68;
    volatile s32 unk7B6C;
    volatile s32 unk7B70;
} Unk8009F4D8;

extern Unk8009F4D8 *D_8009F4D8;
extern void func_80098C68(s32, u32);
extern u32 func_80098788(s32, u32);

void func_800876EC(void)
{
    D_8009F4D8->unk8 = 0;
    func_80098C68(0, D_8009F4D8->unk7B64 | D_8009F4D8->unk7B68);
    func_80098788(D_8009F4D8->unk7B6C, D_8009F4D8->unk7B64);
    func_80098788(D_8009F4D8->unk7B70, D_8009F4D8->unk7B68);
}
