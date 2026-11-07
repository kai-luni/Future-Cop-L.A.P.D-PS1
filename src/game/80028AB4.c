#include "game/types.h"

typedef struct Node {
    s32 unk00;
    s32 unk04;
    u8 pad08[8];
    u8 data[1];
} Node;
typedef struct State {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    u8 pad0c[0x10];
    s32 unk1c;
    u8 unk20;
} State;
extern State D_800A0B70;
extern Node *D_800A0C20[];
extern volatile s32 D_8009F6B4;
extern Node * volatile D_8009F6AC;
extern u8 D_8009F6A8;
extern void func_8001D6D0(s32, s32, s32);
extern void func_8001DB60(s32, void *, s32, void (*)());
extern void func_80028918();

void func_80028AB4(void)
{
    Node *node;
    State *state;
    state = &D_800A0B70;
    node = D_800A0C20[D_8009F6B4];
    if (node != 0 && state->unk20 == 0 && D_800A0B70.unk00 >= 0) {
        D_800A0C20[D_8009F6B4] = 0;
        D_8009F6B4++;
        if (D_8009F6B4 == 4) D_8009F6B4 = 0;
        D_8009F6AC = node;
        D_8009F6AC->unk04 = state->unk08;
        D_8009F6A8 = 1;
        func_8001D6D0(D_800A0B70.unk00, state->unk08, 0);
        func_8001DB60(D_800A0B70.unk00, node->data, state->unk1c, func_80028918);
    }
}
