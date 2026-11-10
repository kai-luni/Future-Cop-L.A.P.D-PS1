#include "game/types.h"

struct Context {
    u8 pad000[0x8f4];
    u32 state;
    s32 field8f8;
    s32 field8fc;
    s32 field900;
    s32 field904;
    u8 pad908[0x9a4 - 0x908];
    s32 field9a4;
    s32 field9a8;
    u8 pad9ac[0x1bb0 - 0x9ac];
    s32 field1bb0;
};
extern struct Context *D_8009AB08;
extern void func_8008D640(s32);

s32 func_8001F784(void)
{
    volatile struct Context *p = D_8009AB08;
    p->field1bb0 = 0;
    p->field9a8 = 1;
    switch (p->state) {
    case 2:
    case 3:
    case 6:
        p->state = 0;
        p->field8f8 = 0;
        p->field904 = 0;
        p->field9a4 = 1;
        func_8008D640(p->field8fc);
        break;
    default:
        ((volatile struct Context *)D_8009AB08)->field8f8 = 0;
        ((volatile struct Context *)D_8009AB08)->state = 0;
        func_8008D640(((volatile struct Context *)D_8009AB08)->field8fc);
        break;
    }
    return 0;
}
