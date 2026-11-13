#include "game/types.h"

typedef struct Node {
    struct Node *next;
    s32 unk04;
    s32 unk08;
    s32 count;
    s32 unk10;
} Node;
typedef struct State {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0c;
    Node *head;
    u8 pad14[8];
    s32 unk1c;
    u8 unk20;
    u8 pad21[0x1b];
    u8 unk3c;
    u8 pad3d[0xf];
    s32 unk4c;
} State;
extern State D_800A0B70;
extern Node *D_800A0C10[];
extern u8 D_8009F6A8;
extern Node * volatile D_8009F6AC;
extern volatile s32 D_8009F6B0;
extern s32 D_8009F6E8;
extern void func_800288BC(Node *);
extern void func_80028AB4(void);

void func_80028918(s32 arg0, s32 arg1)
{
    register State *state asm("$16");
    u8 flag;
    u8 final_flag;
    s32 value;
    Node *node;
    register s32 pos asm("$6");
    register s32 node_value asm("$2");
    if (D_800A0B70.unk00 < 0) {
        D_8009F6A8 = 0;
        return;
    }
    state = &D_800A0B70;
    if (arg0 != state->unk1c) arg1 = 5;
    pos = state->unk08;
    node_value = D_8009F6AC->unk04;
    if (pos != node_value) {
        arg1 = 0xdead;
    } else if ((node_value = arg0 + pos) == state->unk04) {
        arg1 = 0;
        state->unk20 = 1;
        D_8009F6E8 = 0;
    }
    if (arg1 == 0) {
        state->unk08 += arg0;
        if (D_8009F6E8 != 0 && state->unk08 == state->unk4c) {
            D_8009F6E8 = 0;
            state->unk20 = 1;
        }
        flag = 0;
        if (D_8009F6AC->unk10 == 0x53575652 && state->unk0c != -1) {
            if (state->unk3c != 0) {
                flag = 1;
                node = D_8009F6AC;
                __asm__("addu %0,%1,$zero" : "=r"(value) : "r"(flag), "r"(node));
                state->unk20 = value;
                func_800288BC(node);
            } else {
                state->unk3c = 1;
            }
        }
        if (!flag) {
            D_800A0C10[D_8009F6B0] = D_8009F6AC;
            D_8009F6B0++;
            if (D_8009F6B0 == 4) D_8009F6B0 = 0;
        }
    } else {
        func_800288BC(D_8009F6AC);
    }
    final_flag = *(volatile u8 *)&state->unk20;
    D_8009F6AC = 0;
    D_8009F6A8 = 0;
    if (!final_flag) func_80028AB4();
}
