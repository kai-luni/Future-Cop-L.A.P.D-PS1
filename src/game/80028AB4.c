// Categories: Game Engine > Queued Operations
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
extern State gStreamBufferList __asm__("D_800A0B70");
extern Node *D_800A0C20[];
extern volatile s32 D_8009F6B4;
extern Node * volatile D_8009F6AC;
extern u8 D_8009F6A8;
extern void gSeekFileSlot(s32, s32, s32) __asm__("func_8001D6D0");
extern void gReadFileSlot(s32, void *, s32, void (*)()) __asm__("func_8001DB60");
extern void func_80028918();

/**
 *  @brief Starts the next queued node operation when the state permits it.
 *
 *  Removes a node from a four-entry ring and records it as the current node.
 *  The operation uses the current state values and a completion callback.
 *  @par Globals
 *  D_800A0B70 supplies the readiness flag and operation arguments.
 *  D_800A0C20 and D_8009F6B4 provide the ring and its read index.
 *  D_8009F6AC records the current node; D_8009F6A8 is set to one.
 *  @par Control flow
 *  Proceeds only for a nonnull node, a clear state flag, and a nonnegative handle.
 *  Advances the read index with wraparound at four.
 *  @par Calls
 *  func_8001D6D0 prepares the operation; func_8001DB60 starts it with node data.
 *  func_80028918 is passed as the completion callback.
 *  @par Side effects
 *  Clears a ring entry, updates queue state and the node, and starts an operation.
 *  @par Callers
 *  Confirmed: func_80028918, func_80028E58.
 */
void func_80028AB4(void)
{
    Node *node;
    State *state;
    state = &gStreamBufferList;
    node = D_800A0C20[D_8009F6B4];
    if (node != 0 && state->unk20 == 0 && gStreamBufferList.unk00 >= 0) {
        D_800A0C20[D_8009F6B4] = 0;
        D_8009F6B4++;
        if (D_8009F6B4 == 4) D_8009F6B4 = 0;
        D_8009F6AC = node;
        D_8009F6AC->unk04 = state->unk08;
        D_8009F6A8 = 1;
        gSeekFileSlot(gStreamBufferList.unk00, state->unk08, 0);
        gReadFileSlot(gStreamBufferList.unk00, node->data, state->unk1c, func_80028918);
    }
}
