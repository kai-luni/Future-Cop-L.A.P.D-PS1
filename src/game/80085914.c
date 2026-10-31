#include "game/types.h"

typedef struct {
    u8 unk00[0x34];
    u8 unk34[2][92];
    u8 unkEC[2][20];
    s32 unk114;
    u8 unk118[4];
    s32 unk11C;
} Unk8009F4BC;

extern Unk8009F4BC *D_8009F4BC;
extern void func_80092C8C(s32);
extern void func_80091270(s32);
extern void func_800934BC(void *);
extern void func_800932F0(void *);

void func_80085914(void)
{
    s32 t;

    if (D_8009F4BC->unk11C != 0) {
        func_80092C8C(0);
        func_80091270(0);
        func_800934BC((void *)&D_8009F4BC->unkEC[D_8009F4BC->unk114][0]);
        t = 1 - D_8009F4BC->unk114;
        D_8009F4BC->unk114 = t;
        func_800932F0((void *)&D_8009F4BC->unk34[t][0]);
    }
}
