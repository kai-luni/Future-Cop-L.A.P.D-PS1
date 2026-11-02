#include "game/types.h"

struct Unk52E28 {
    u8 pad_00[0xB0];
    s16 unkB0;
    u8 pad_B2[6];
    u8 unkB8;
};

void func_80052E28(struct Unk52E28 *self)
{
    register s32 target __asm__("$5") = 0;
    s32 delta;

    switch (self->unkB8 & 7) {
    case 1:
        if (self->unkB0 < 0x400) {
            target = 0;
        } else {
            target = 0x800;
            if (self->unkB0 >= 0xC01) {
                target = 0x1000;
            }
        }
        break;
    case 2:
        if (self->unkB0 < 0x800) {
            target = 0x400;
        } else {
            target = 0xC00;
        }
        break;
    case 3:
        if (self->unkB0 > 0xA00) {
            target = 0xE00;
        } else {
            if (self->unkB0 < 0x200) {
                target = -1;
            } else {
                target = 0x600;
            }
        }
        break;
    case 4:
        if (self->unkB0 < 0x600) {
            target = 0x200;
        } else {
            target = 0xA00;
            if (self->unkB0 >= 0xE01) {
                target = 0x1000;
            }
        }
        break;
    }

    delta = target - self->unkB0;
    target = 8;
    if (delta > 0) {
        if (target < delta) {
            delta = target;
        }
    }
    if (delta < 0) {
        if (target < -delta) {
            delta = -target;
        }
    }

    self->unkB0 += delta;
}
