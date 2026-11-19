// Categories: Game Engine > World Reset | Game Engine > Entity Lifecycle | Resources > Memory Management
#include "game/types.h"

struct Unk3CF20 {
    struct Unk3CF20 *next;
};

extern struct Unk3CF20 *gAnchorNodeList __asm__("D_8009F320");
extern void func_8003CF20(struct Unk3CF20 *object);

/**
 * @brief Tears down every node in the D_8009F320 object list and clears the list head.
 *
 * Walks the singly-linked list headed by D_8009F320, capturing object->next (+0x00) before
 * releasing each node via func_8003CF20, then sets D_8009F320 to 0.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F320 - head pointer of the transform-sync object list (read, drained, cleared to 0).
 *
 * @par Control flow
 * Single while-loop over object != 0 saving object->next before calling func_8003CF20.
 *
 * @par Calls
 * func_8003CF20 - frees an object's owned transform buffer (if flag bit 1 is set) and the node itself.
 *
 * @par Side effects
 * Destroys all nodes on D_8009F320 via func_8003CF20 and zeroes D_8009F320.
 *
 * @par Callers
 * Confirmed: func_8003F854 (0x8003F9F4, during world teardown) / Likely: none / Unknown: none.
 */
void func_8003D0AC(void)
{
    struct Unk3CF20 *object;
    struct Unk3CF20 *next;

    object = gAnchorNodeList;
    while (object != 0) {
        next = object->next;
        func_8003CF20(object);
        object = next;
    }
    gAnchorNodeList = 0;
}
