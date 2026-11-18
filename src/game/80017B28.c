// Categories: Graphics > Model Renderer > Instance Lookup
#include "game/types.h"

struct Unk17764 {
    u8 unk00[0x88];
    s32 id;
    struct Unk17764 *next;
};

extern struct Unk17764 *gCameraList __asm__("D_8009F588");
struct Unk17764 *gFindInstanceById(s32 id) __asm__("func_80017B28");


/**
 * @brief Finds a draw instance by id in the active list.
 *
 * @param id The instance id to match (+0x88 of each node).
 * @return The first node whose id matches, or 0.
 *
 * @par Confirmed
 * Linear walk of the singly linked list at D_8009F588 (next at +0x8C),
 * comparing the id word at +0x88; no writes.
 *
 * @par Likely role
 * The lookup behind func_80017B60, which selects the draw instance for a
 * given id before submitting it to the renderer.
 *
 * @par Callers
 * func_80017B60.
 *
 * @par Environment path
 * Resolves a draw instance by its id (+0x88); the view record selection uses
 * it to turn the passed id into the active record.
 */
struct Unk17764 *gFindInstanceById(s32 id)
{
    struct Unk17764 *node;

    for (node = gCameraList; node != 0; node = node->next) {
        if (id == node->id) {
            return node;
        }
    }
    return 0;
}
