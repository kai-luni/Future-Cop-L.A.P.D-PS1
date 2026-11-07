#include "game/types.h"
typedef struct Node { struct Node *next; } Node;
typedef struct State { u8 pad[0x10]; Node *pending; Node *head; u8 pad18[8]; u8 flag; } State;
extern State D_800A0B70;
extern Node *D_800A0C20[];
extern Node *D_800A0C10[];
extern s32 D_8009F6A4;
extern s32 D_8009F6A0;
extern s32 D_8009F6B4;
extern u8 D_8009F6A8;
extern void func_80028AB4(void);
Node *func_80028E58(u8 arg)
{
    State *state = &D_800A0B70;
    Node *node;
    Node *p;
    if (state->flag == 0) {
        while ((node = state->pending) != 0) {
            state->pending = node->next;
            node->next = 0;
            D_800A0C20[D_8009F6A4] = node;
            D_8009F6A4++;
            if (D_8009F6A4 == 4) D_8009F6A4 = 0;
        }
        if (D_800A0C20[D_8009F6B4] != 0 && D_8009F6A8 == 0) func_80028AB4();
    }
    if (arg) {
        while ((node = D_800A0C10[D_8009F6A0]) != 0) {
            D_800A0C10[D_8009F6A0] = 0;
            D_8009F6A0++;
            if (D_8009F6A0 == 4) D_8009F6A0 = 0;
            node->next = 0;
            p = state->head;
            if (p == 0) state->head = node;
            else {
                while (p->next != 0) p = p->next;
                p->next = node;
            }
        }
    }
    return state->head;
}
