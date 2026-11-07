#include "game/types.h"

typedef struct {
    u8 pad[8];
    s32 entries[257];
    volatile u32 index;
    volatile s32 count;
} Queue;
extern Queue *D_8009F970;
extern s32 func_8008E2F8(s32, s32);

s32 func_8008888C(s32 arg0, s32 arg1, s32 arg2)
{
    register s32 b __asm__("$17") = arg1;
    register s32 a __asm__("$16") = arg0;
    register Queue *queue __asm__("$5") = D_8009F970;
    {
        register u32 index __asm__("$4") = queue->index;
        register s32 value __asm__("$3") = a & 0xff;
        queue->entries[index++] = value;
        queue->index = index;
    }
    queue->entries[queue->index++] = b;
    queue->entries[queue->index++] = arg2;
    queue->index %= 48;
    queue->count++;
    if (queue->count == 1) {
        while (!func_8008E2F8(a & 0xff, b)) {
        }
    }
    return 1;
}
