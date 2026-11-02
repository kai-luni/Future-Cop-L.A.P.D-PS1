#include "game/types.h"

struct StreamState {
    u8 flag;
    u8 unk01[0x0F];
    s16 unk10;
    u8 unk12[0x6E];
    s32 ready;
};

extern struct StreamState D_800A0968;
extern s32 D_800A0970[2];
extern u16 D_8009F268;

extern void func_80022CA8(void);
extern void func_80022D84(void);

s32 func_80022710(void)
{
    s32 i;
    s32 j;
    struct StreamState *p;
    struct StreamState *q;

    i = 0xFFFF;
    func_80022CA8();
    p = &D_800A0968;
    p->flag = 1;
    D_8009F268++;
    if (D_8009F268 == 0) {
        D_8009F268 = 1;
    }
    p->unk10 = D_8009F268;
    func_80022D84();

    if (p->ready == 0) {
        q = p;

        do {
            j = i;
            i--;
        } while (j != 0 && q->ready == 0);
    }
    if (i != 0) {
        D_800A0970[0] = 1;
    }
    return D_800A0970[0];
}
