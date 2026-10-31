#include "game/types.h"

struct Unk8007F9CC {
    u8 pad0[0x200];
    u8 unk200;
    u8 pad201[0x7];
    u32 unk208;
    u8 pad20C[0x6F8];
    void *volatile unk904;
    u8 pad908[0x3D];
    u8 unk945;
    u8 pad946[0x2];
};

extern struct Unk8007F9CC *D_8009F92C;

extern void *func_80017EBC(s32 size, s32 tag);
extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern int func_80020610(void);
extern void func_80020618(s32 arg0, void *arg1);

void func_8007F9CC(void)
{
    void *p;
    void *q;
    struct Unk8007F9CC *s;

    p = func_80017EBC(0x948, 2);
    D_8009F92C = p;
    func_8001E4C0(p, 0, 0x948);
    D_8009F92C->unk208 = 3;
    D_8009F92C->unk945 = 2;
    q = func_80017EBC(func_80020610(), 2);
    s = D_8009F92C;
    s->unk200 = 1;
    s->unk904 = q;
    func_80020618(0, D_8009F92C->unk904);
}
