// Categories: Graphics > Effects > Beams | Graphics > Effects > Object Pool
#include "game/types.h"

extern int gBeamPartUpdate();

struct ListNode {
    struct ListNode *previous;
    struct ListNode *next;
    struct ListNode *tail;
};

extern struct ListNode gTracerPool __asm__("D_800A3538");

extern struct ListNode *gTakePoolNode(struct ListNode *node) __asm__("func_8003BEF0");
extern void func_8006EEF8(struct ListNode *node, s32 arg0, s16 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5);

/**
 * @brief Spawns one beam/tracer node from the effect pool and aims it between
 *        two points.
 *
 * @param arg0 Scale forwarded to the node initialiser.
 * @param arg1 s16 parameter forwarded unchanged.
 * @param arg2, arg3 Forwarded unchanged.
 * @param arg4, arg5 Pointers to the start and end position triples.
 * @return Always 1, even when the pool is exhausted.
 *
 * @par Confirmed
 * func_8003BEF0(&D_800A3538) takes a free node from the pool; when one is
 * available func_8006EEF8(node, arg0, arg1, arg2, arg3, arg4, arg5) fills it:
 * the direction arg5 - arg4 is normalised (func_8001A314) and scaled by
 * arg0 >> 12 into the node's velocity words +0x34/+0x36/+0x38.
 *
 * @par Likely role
 * The tracer spawn used by the actor dispatcher func_8005F6E8 and the
 * effect handler gBeamPartUpdate: a stretched beam between muzzle and impact.
 *
 * @par Unknown
 * The pool capacity of D_800A3538 and the meaning of arg1..arg3 in the
 * initialiser.
 *
 * @par Callers
 * func_8005F6E8, gBeamPartUpdate.
 */
s32 func_8006F0E8(s32 arg0, s16 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5)
{
    struct ListNode *node = gTakePoolNode(&gTracerPool);

    if (node != 0) {
        func_8006EEF8(node, arg0, arg1, arg2, arg3, arg4, arg5);
    }
    return 1;
}
