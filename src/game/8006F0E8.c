#include "game/types.h"

struct ListNode {
    struct ListNode *previous;
    struct ListNode *next;
    struct ListNode *tail;
};

extern struct ListNode D_800A3538;

extern struct ListNode *func_8003BEF0(struct ListNode *node);
extern void func_8006EEF8(struct ListNode *node, s32 arg0, s16 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5);

s32 func_8006F0E8(s32 arg0, s16 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5)
{
    struct ListNode *node = func_8003BEF0(&D_800A3538);

    if (node != 0) {
        func_8006EEF8(node, arg0, arg1, arg2, arg3, arg4, arg5);
    }
    return 1;
}
