// Categories: Graphics > Scene Graph > Object List Walk | Graphics > Model Renderer > Part Animation
#include "game/types.h"

struct Unk7364C {
    struct Unk7364C *next;
    u8 unk04[0x0A];
    u16 flags;
};

extern struct Unk7364C *gBeamNodeList __asm__("D_8009F38C");

extern void gFreeBeamNode(struct Unk7364C *node) __asm__("func_80071A68");
extern void gStepBeamNode(struct Unk7364C *node, s32 arg) __asm__("func_80071B40");
void gTickBeamNodeList(s32 arg) __asm__("func_8007364C");


/**
 * @brief Per-frame pass over the list at D_8009F38C: retires flagged
 *        nodes, animates active ones and activates new ones.
 *
 * @param arg Forwarded to the animation call.
 * @return Nothing.
 *
 * @par Confirmed
 * Walks the singly linked list with a previous pointer. flags & 0x1000:
 * unlink the node (previous->next or the list head), clear node->next and
 * call func_80071A68(node). flags & 0x800: func_80071B40(node, arg) (part
 * animation) and keep the node. Otherwise set flags |= 0x800 and keep it.
 *
 * @par Likely role
 * 0x800 marks "active since the previous pass", 0x1000 marks "remove";
 * new nodes get one activation frame before they animate.
 *
 * @par Callers
 * func_8003F4A0 (scene traversal).
 *
 * @par Environment path
 * Per-frame maintenance of the beam/ribbon node list D_8009F38C (nodes built
 * by func_8007377C): walking with a predecessor, a node whose u16 flags at
 * +0x0E have 0x1000 is unlinked and destroyed with func_80071A68 (which frees
 * its two pooled blocks and entry array), a node with 0x800 is advanced by
 * gStepBeamNode (func_80071B40) with the frame argument, and any other node
 * only gets 0x800 so it starts stepping on the next pass. It is the first
 * call of gRunSceneListPasses (func_8003F4A0, 8003F4A0.c:43) on the tick
 * side; gDrawBeamNodeList (func_80073700) draws the same list per view.
 */
void gTickBeamNodeList(s32 arg)
{
    struct Unk7364C *node;
    register struct Unk7364C *previous __asm__("$18");
    register struct Unk7364C *next __asm__("$17");
    u16 flags;

    previous = 0;
    for (node = gBeamNodeList; node != 0; node = next) {
        flags = node->flags;
        next = node->next;
        if (flags & 0x1000) {
            if (previous != 0) {
                previous->next = next;
            } else {
                gBeamNodeList = next;
            }
            node->next = 0;
            gFreeBeamNode(node);
        } else if (flags & 0x800) {
            gStepBeamNode(node, arg);
            previous = node;
        } else {
            node->flags = flags | 0x800;
            previous = node;
        }
    }
}
