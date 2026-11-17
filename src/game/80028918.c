// Categories: Game Engine > Node Queues
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
extern State gStreamBufferList __asm__("D_800A0B70");
extern Node *D_800A0C10[];
extern u8 D_8009F6A8;
extern Node * volatile D_8009F6AC;
extern volatile s32 D_8009F6B0;
extern s32 D_8009F6E8;
extern void func_800288BC(Node *);
extern void func_80028AB4(void);

/**
 *  @brief Handles completion of the current queued node operation.
 *
 *  The routine checks the reported amount and current position, updates progress
 *  on success, and either queues the node as completed or passes it to cleanup.
 *  A special node tag and state flags can terminate further processing.
 *  @param arg0 Reported amount used to validate and advance the position.
 *  @param arg1 Reported status, subject to overrides by the validation checks.
 *  @par Globals
 *  D_800A0B70 holds progress and flags; D_8009F6AC is the current node.
 *  D_800A0C10 and D_8009F6B0 hold the completion queue and write index.
 *  D_8009F6A8 is the busy flag; D_8009F6E8 controls an additional end check.
 *  @par Control flow
 *  An inactive state clears busy and returns; otherwise validation selects success
 *  or cleanup, and a final flag determines whether processing resumes.
 *  @par Calls
 *  func_800288BC handles rejected or special nodes; func_80028AB4 resumes processing.
 *  @par Side effects
 *  Updates progress, flags, queue entries, and indices; clears the current node and busy flag.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80028918(s32 arg0, s32 arg1)
{
    register State *state asm("$16");
    u8 flag;
    u8 final_flag;
    s32 value;
    Node *node;
    register s32 pos asm("$6");
    register s32 node_value asm("$2");
    if (gStreamBufferList.unk00 < 0) {
        D_8009F6A8 = 0;
        return;
    }
    state = &gStreamBufferList;
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
