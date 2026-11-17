// Categories: Gameplay > Map | Resources > Memory Management
#include "game/types.h"

struct GridObject {
    s16 unk00;
    s16 cell;
    u8 unk04[0x3C];
    struct GridObject *next;
};

extern struct GridObject **gLightGrid __asm__("D_8009F780");
extern s32 gLightNodePool __asm__("D_8009F784");

extern void func_800195D0(s32 pool, struct GridObject *object);

/**
 * @brief Unlinks a grid object from its map cell list and returns it to the
 *        pool.
 *
 * @param object Grid object; +2 the s16 cell index, +0x40 the next pointer.
 * @return Nothing.
 *
 * @par Confirmed
 * For a non-negative cell, removes the object from the singly linked list
 * at D_8009F780[cell] (head replacement or predecessor search), then always
 * calls func_800195D0(D_8009F784, object) to release it into the pool.
 *
 * @par Likely role
 * The despawn path of map-bound objects: the effect and entity handlers
 * func_8003F2AC/func_80040874/func_80075478 call it when an object leaves
 * the map or expires.
 *
 * @par Callers
 * func_8003F2AC, func_80040874, func_80075478.
 */
void func_800498F0(struct GridObject *object)
{
    struct GridObject **cell;
    struct GridObject *node;

    if (object->cell >= 0) {
        cell = &gLightGrid[object->cell];
        node = *cell;
        if (node == object) {
            *cell = object->next;
        } else {
            while (node != 0) {
                if (node->next == object) {
                    node->next = object->next;
                    break;
                }
                node = node->next;
            }
        }
    }
    func_800195D0(gLightNodePool, object);
}
