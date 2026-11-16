// Categories: Graphics > Scene Graph | Game Engine > World Reset
#include "game/types.h"

struct Unk734B0 {
    struct Unk734B0 *next;
};

extern struct Unk734B0 *D_8009F38C;

extern void func_80073BA8(struct Unk734B0 *node);

/**
 * @brief Marks every node on the D_8009F38C list with retire flag 0x1000 and clears the list head.
 *
 * Walks the singly-linked list headed by D_8009F38C, saving node->next (+0x00) before calling
 * func_80073BA8(node) to set bit 0x1000 in each node's flags (+0x0E), then zeroes D_8009F38C.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F38C - head pointer of the scene-graph part/effect object list (walked and cleared to 0).
 *
 * @par Control flow
 * Single while-loop over node != 0 saving node->next prior to invoking func_80073BA8.
 *
 * @par Calls
 * func_80073BA8 - sets bit 0x1000 in node->flags (+0x0E) without freeing the node directly.
 *
 * @par Side effects
 * Sets flag bit 0x1000 on every node previously reachable from D_8009F38C and clears D_8009F38C = 0.
 *
 * @par Callers
 * Confirmed: func_80024D14 (0x80024D38, first reset call in between-mission global reset) / Likely: none / Unknown: none.
 */
void func_800734BC(void)
{
    struct Unk734B0 *node;
    struct Unk734B0 *next;

    node = D_8009F38C;
    while (node != 0) {
        next = node->next;
        func_80073BA8(node);
        node = next;
    }
    D_8009F38C = 0;
}
