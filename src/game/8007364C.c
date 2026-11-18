// Categories: Graphics > Scene Graph > Object List Walk | Graphics > Model Renderer > Part Animation
#include "game/types.h"

struct Unk7364C {
    struct Unk7364C *next;
    u8 unk04[0x0A];
    u16 flags;
};

extern struct Unk7364C *gModelNodeList __asm__("D_8009F38C");

extern void func_80071A68(struct Unk7364C *node);
extern void gStepBeamNode(struct Unk7364C *node, s32 arg) __asm__("func_80071B40");

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
 */
void func_8007364C(s32 arg)
{
    struct Unk7364C *node;
    register struct Unk7364C *previous __asm__("$18");
    register struct Unk7364C *next __asm__("$17");
    u16 flags;

    previous = 0;
    for (node = gModelNodeList; node != 0; node = next) {
        flags = node->flags;
        next = node->next;
        if (flags & 0x1000) {
            if (previous != 0) {
                previous->next = next;
            } else {
                gModelNodeList = next;
            }
            node->next = 0;
            func_80071A68(node);
        } else if (flags & 0x800) {
            gStepBeamNode(node, arg);
            previous = node;
        } else {
            node->flags = flags | 0x800;
            previous = node;
        }
    }
}
