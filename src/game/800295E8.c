// Categories: Resources > Asset Loader | Resources > File Access
#include "game/types.h"

struct LinkState {
    unsigned char pad_00[0xC];
    int count;
    int *link;
};

struct Node {
    struct Node *next;
    s32 value;
};

struct Data_800A0B70 {
    u8 pad_00[0x2C];
    struct LinkState *state;
    struct Node *link;
};

extern struct Data_800A0B70 gStreamBufferList __asm__("D_800A0B70");

extern void func_8002902C(u8 arg);
extern void func_800195D0(struct LinkState *state, int *link);

/**
 * @brief Pops one pending asset request from the queue inside D_800A0B70,
 *        optionally advancing the bank/stream parser first.
 *
 * The queue is a singly linked list of two-word entries rooted at
 * D_800A0B70+0x30: the successor at +0x0 and the request block at +0x4. A pop
 * takes the request block, writes the successor back into the head, returns the
 * entry to the pool whose state sits at +0x2C and hands the block to the caller,
 * which files it into its own dispatch list (func_80024EC8). A non-zero argument
 * runs the parser first, which is what appends new requests.
 *
 * @param arg Non-zero to run the parser before popping; only the low 8 bits are
 *        tested.
 * @return The request block, or 0 when the queue is empty.
 *
 * @par Globals
 * D_800A0B70+0x30 - queue head, replaced by the popped entry's successor.
 * D_800A0B70+0x2C - allocator state for the two-word entries.
 *
 * @par Control flow
 * Optional parser call, null test on the head, three loads, head update, free,
 * return of the saved block pointer.
 *
 * @par Calls
 * func_8002902C - walks the CHOc/CTRL/CANm/FILL/VAGB/VAGM container of the module
 * stream and dispatches its chunks (VAG banks, sound data).
 * func_800195D0 - returns the popped entry to its pool.
 *
 * @par Side effects
 * Advances the queue head and frees one entry; the returned block changes owner.
 *
 * @par Callers
 * Confirmed: func_80024EC8, which drains the queue before dispatching the level
 * requests.
 */
s32 func_800295E8(u8 arg)
{
    struct Node *node;
    struct Node *next;
    struct LinkState *state;
    s32 value;

    if (arg != 0) {
        func_8002902C(arg);
    }
    node = gStreamBufferList.link;
    if (node == 0) {
        return 0;
    }
    value = node->value;
    next = node->next;
    state = gStreamBufferList.state;
    gStreamBufferList.link = next;
    func_800195D0(state, (int *)node);
    return value;
}
