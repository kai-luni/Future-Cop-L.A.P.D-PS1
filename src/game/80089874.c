#include "game/types.h"

typedef struct {
    u8 pad0[0x420];
    s32 unk420;
    u8 pad424[0x150];
    void *unk574;
    void *unk578;
} Unk8009F970;

extern Unk8009F970 *D_8009F970;
extern void func_8008E5D8(void *);
extern void func_80091240(void *);
extern void func_800895E4(s32);

void func_80089874(void)
{
    if (D_8009F970->unk420 != 0) {
        func_8008E5D8(D_8009F970->unk574);
        func_80091240(D_8009F970->unk578);
        func_800895E4(0);
    }
}
