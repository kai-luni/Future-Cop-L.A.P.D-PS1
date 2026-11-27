// Categories: Graphics > Scene Graph | Game Engine > World Reset
#include "game/types.h"

struct Unk734B0 {
    struct Unk734B0 *next;
};

extern struct Unk734B0 *gBeamNodeList __asm__("D_8009F38C");

extern void gSetObjectFlag1000(struct Unk734B0 *node) __asm__("func_80073BA8");
void gRetireAllRibbons(void) __asm__("func_800734BC");


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
 *
 * @par Environment path
 * Walks the ribbon/beam node list D_8009F38C (pushed by the constructor
 * func_8007377C, 8007377C.c:185-186), sets retire flag 0x1000 on each node
 * through func_80073BA8 and then clears the list head. Its only caller is
 * gResetMissionSubsystems (func_80024D14), the between-mission reset. Because
 * the head is cleared immediately, the per-frame list pass func_8007364C that
 * normally retires flagged nodes never sees them again, so this only detaches
 * the nodes; it frees nothing itself.
 */
void gRetireAllRibbons(void)
{
    struct Unk734B0 *node;
    struct Unk734B0 *next;

    node = gBeamNodeList;
    while (node != 0) {
        next = node->next;
        gSetObjectFlag1000(node);
        node = next;
    }
    gBeamNodeList = 0;
}
