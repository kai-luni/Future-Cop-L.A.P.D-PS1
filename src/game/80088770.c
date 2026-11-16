// Categories: Platform > Device Command Queue
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

/**
 * @brief Appends a three-word command to the 48-word ring buffer.
 *
 * The device lock is taken, the three values are appended (the first masked
 * to 8 bits), the index wraps modulo 48 and the pending counter grows. When
 * this was the first pending command the lock is released and the transfer
 * func_8008E1BC is retried until it reports success; otherwise only the lock
 * is released and the command waits for the consumer.
 *
 * @param arg0 Command command byte (masked with 0xFF).
 * @param arg1 Second command word.
 * @param arg2 Third command word.
 * @return Always 1.
 *
 * @par Globals
 * D_8009F970 - queue block: entries from 0x08, index and count after 257 words.
 *
 * @par Control flow
 * Lock, three stores, wraparound, then one blocking or one non-blocking exit.
 *
 * @par Calls
 * func_8008C9C0/func_8008C9E0 - lock and unlock. func_8008E1BC - device transfer.
 *
 * @par Side effects
 * Writes three queue words, the index and the count.
 *
 * @par Callers
 * Unknown: no direct edge; the caller is probably a table-driven dispatcher.
 */
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
