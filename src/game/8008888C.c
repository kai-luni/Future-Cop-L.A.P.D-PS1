// Categories: Game Engine > Command Queue
#include "game/types.h"

typedef struct {
    u8 pad[8];
    s32 entries[257];
    volatile u32 index;
    volatile s32 count;
} Queue;
extern Queue *gMovieStreamContext __asm__("D_8009F970");
extern s32 func_8008E2F8(s32, s32);

/**
 *  @brief Enqueues a three-value command and starts it if the queue was empty.
 *
 *  Stores the low byte of arg0 followed by arg1 and arg2 in the queue.
 *  The write index is reduced modulo 48 after all three entries are written.
 *  @param arg0 Command value, of which only the low byte is stored and sent.
 *  @param arg1 Second queued value and second dispatch argument.
 *  @param arg2 Third queued value.
 *  @return Always 1.
 *  @par Globals
 *  gMovieStreamContext points to the queue whose entries, index, and count are updated.
 *  @par Control flow
 *  If the incremented count is one, retries dispatch until it succeeds.
 *  @par Calls
 *  func_8008E2F8 attempts to dispatch the first two command values.
 *  @par Side effects
 *  Modifies the queue and may busy-wait indefinitely for dispatch.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_8008888C(s32 arg0, s32 arg1, s32 arg2)
{
    register s32 b __asm__("$17") = arg1;
    register s32 a __asm__("$16") = arg0;
    register Queue *queue __asm__("$5") = gMovieStreamContext;
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
