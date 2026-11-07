#include "game/types.h"

struct Data {
    u8 pad0[4];
    u8 kind;
    u8 flags;
    s16 value1;
    u8 pad8[2];
    u8 value2;
    u8 padB[0x1b];
    s16 value3;
};
struct State {
    u8 pad0[0x68];
    s32 field68;
    s32 field6c;
    s32 field70;
    u8 pad74[4];
    s32 field78[3];
    s16 field84[3];
    s16 field8a[3];
    u8 field90[3];
    u8 field93[3];
    u8 pad96[2];
    s32 field98;
    u8 pad9c[9];
    u8 fielda5;
    u8 pada6[0x1b];
    s8 fieldc1;
};
struct Owner { u8 pad[0xac]; struct State *state; };
struct Global { u8 pad[8]; s32 field8; u8 padc[0x8c]; s32 field98; };
extern struct Global *D_8009FA94;
extern struct Data *func_8006AAEC(u8);
extern void func_8005D3FC(struct Owner *, s32);

void func_8006AB38(struct Owner *owner)
{
    struct State *state;
    struct Data *data;
    s32 i, mode;
    register s32 a __asm__("$5");
    register s32 b __asm__("$4");
    state = owner->state;
    for (i = 0; i < 3; i++) {
        data = func_8006AAEC(state->field90[i]);
        a = data->value1;
        b = data->value3;
        if (owner->state->fieldc1 == 0)
            mode = D_8009FA94->field8;
        else
            mode = D_8009FA94->field98;
        switch (mode) {
        case 0:
            break;
        case 1:
            a += a >> 2;
            b += b >> 2;
            break;
        case 2:
            a += a >> 1;
            b += b >> 1;
            break;
        case 3:
            b = a = 9999;
            break;
        }
        state->field84[i] = a;
        state->field8a[i] = b;
        state->field93[i] = data->value2;
        state->field78[i] = 0;
        if (data->flags & 4)
            state->fielda5 = 0;
        if (data->kind == 13)
            func_8005D3FC(owner, i);
    }
    state->field98 = 0;
    state->field6c = 0;
    state->field70 = 0;
    state->field68 = -1;
}
