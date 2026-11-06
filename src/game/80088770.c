#include "game/types.h"

typedef struct {
    u8 pad[8];
    s32 entries[257];
    volatile u32 index;
    volatile s32 count;
} Queue;

extern Queue *D_8009F970;
extern void func_8008C9C0(void);
extern void func_8008C9E0(void);
extern s32 func_8008E1BC(s32, s32, s32);

s32 func_80088770(s32 arg0, s32 arg1, s32 arg2)
{
    register s32 b __asm__("$17") = arg1;
    register s32 c __asm__("$18") = arg2;
    register s32 a __asm__("$16") = arg0;
    Queue *queue;
    func_8008C9C0();
    queue = D_8009F970;
    queue->entries[queue->index++] = a & 0xff;
    queue->entries[queue->index++] = b;
    queue->entries[queue->index++] = c;
    queue->index %= 48;
    queue->count++;
    if (queue->count == 1) {
        func_8008C9E0();
        while (!func_8008E1BC(a & 0xff, b, c)) {
        }
    } else {
        func_8008C9E0();
    }
    return 1;
}
