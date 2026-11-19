// Categories: Gameplay > Map > Occupancy | Game Engine > Deferred Queue
#include "game/types.h"

struct Unk3FA34Link {
    struct Unk3FA34Link *next;
    void *object;
};

extern void *gDeferredRegPool __asm__("D_8009F70C");
extern struct Unk3FA34Link *gDeferredRegList __asm__("D_8009F708");
extern s32 gScreenState[] __asm__("D_8009FA00");

extern void func_8003BC0C(void *object);
extern u8 func_8003B838(void *object);
extern struct Unk3FA34Link *func_800195A4(void *pool);

/**
 * @brief Applies or defers map-occupancy registration for a newly spawned object.
 *
 * Immediately calls func_8003BC0C when the deferred queue pool D_8009F70C is absent
 * or D_8009FA00[0] is 4 or 0xD; otherwise queues an 8-byte link node onto D_8009F708.
 *
 * @param object Spawned entity pointer passed to func_8003BC0C or stored in the link node.
 *
 * @par Globals
 * D_8009F70C - pool of 8-byte Unk3FA34Link nodes; D_8009F708 - singly-linked queue head; D_8009FA00 - screen/game state array.
 *
 * @par Control flow
 * Branches to func_8003BC0C and returns if D_8009F70C == 0 or D_8009FA00[0] is 4 or 0xD; else checks func_8003B838(object), pops a node via func_800195A4, and prepends it to D_8009F708.
 *
 * @par Calls
 * func_8003BC0C (0x8003FA68) - updates map occupancy immediately; func_8003B838 (0x8003FA78) - checks valid slot kind; func_800195A4 (0x8003FA90) - allocates link node from D_8009F70C.
 *
 * @par Side effects
 * May write map occupancy via func_8003BC0C or consume a node from D_8009F70C and prepend to D_8009F708.
 *
 * @par Callers
 * Confirmed: func_8002A5F4 (0x8002A7EC, base spawn), func_80039504 (0x80039588, class-5 child registration).
 */
void func_8003FA34(void *object)
{
    struct Unk3FA34Link *link;

    if (gDeferredRegPool == 0 || gScreenState[0] == 4 || gScreenState[0] == 0xD) {
        func_8003BC0C(object);
        return;
    }
    if (func_8003B838(object) != 0) {
        link = func_800195A4(gDeferredRegPool);
        if (link != 0) {
            link->object = object;
            if (gDeferredRegList == 0) {
                link->next = 0;
            } else {
                link->next = gDeferredRegList;
            }
            gDeferredRegList = link;
        }
    }
}
