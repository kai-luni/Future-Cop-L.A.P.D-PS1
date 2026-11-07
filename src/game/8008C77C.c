#include "game/types.h"

struct Obj {
    u8 pad0[0x30];
    u8 *out;
    u8 pad34[8];
    u8 *cmd;
    u8 pad40[6];
    u8 state;
    u8 pad47[2];
    u8 flag;
    u8 count;
    u8 pad4b;
    s32 timer;
    u8 pad50[0x98];
    u8 active;
};
extern void (*D_8009C72C)(struct Obj *);

void func_8008C77C(struct Obj *self)
{
    self->timer++;
    if (self->state != 0) {
        if (self->state == 1) {
            if (self->count < 2) {
                self->count++;
            } else {
                self->flag = 2;
                self->state = 255;
            }
            return;
        }
        if (self->count < 4) {
            self->count++;
            return;
        }
        if (self->flag != 0) {
            D_8009C72C(self);
        }
    }
    if (*self->cmd != 0xf3) {
        self->out[0] = 255;
        self->out[1] = 0;
        self->active = 0;
    }
}
