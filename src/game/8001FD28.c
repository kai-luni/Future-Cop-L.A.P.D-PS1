#include "game/types.h"

typedef struct {
    u8 pad0[0x8F4];
    s32 state;
    s32 mode;
    s32 handle;
    s32 result;
    s32 flag;
    u8 pad908[0x9C];
    s32 active;
    s32 enabled;
    u8 pad9AC[0x1204];
    s32 reset;
} State;

extern State *D_8009AB08;
extern void func_8008D640(s32);
extern void func_8008D690(s32);

s32 func_8001FD28(void)
{
    register State *p asm("$2");
    State *q;
    D_8009AB08->reset = 0;
    D_8009AB08->enabled = 1;
    switch (D_8009AB08->state) {
    case 2:
    case 3:
    case 6:
        p = D_8009AB08;
        p->result = -3;
        p->mode = 0;
        p->state = 0;
        p->flag = 0;
        p->active = 1;
        asm volatile("" : : : "memory");
        goto finish;
    case 1:
        q = D_8009AB08;
        q->result = -1;
        asm volatile("" : : : "memory");
        if (q->mode == 5) {
            q->flag = 1;
            q->mode = 6;
            asm volatile("" : : : "memory");
        } else {
            q->mode = 0;
        }
    case 4:
        p = D_8009AB08;
        p->state = 0;
    finish:
        func_8008D640(p->handle);
        break;
    default:
        p = D_8009AB08;
        p->result = -2;
        p->mode = 0;
        p->state = 4;
        p->flag = 0;
        asm volatile("" : : : "memory");
        func_8008D690(p->handle);
        break;
    }
    return 0;
}
