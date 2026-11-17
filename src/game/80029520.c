// Categories: Resources > Asset Registry | Resources > Reference Counting
#include "game/types.h"

typedef struct {
    s32 unk00[2];
    s32 unk08[2];
    u8 count10;
    u8 count11;
    u8 unk12;
    u8 unk13;
} Data800A0C30;

extern Data800A0C30 gReleaseQueues __asm__("D_800A0C30");

extern void func_80029650(s32 arg0);

/**
 * @brief Flushes both deferred asset-handle release queues in D_800A0C30.
 *
 * Iterates over the queued handle arrays unk00[0..count10-1] and unk08[0..count11-1]
 * in D_800A0C30, releasing each via func_80029650, then resets queue counts and
 * status bytes (unk12 = 0, unk13 = 0xFF).
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_800A0C30 - deferred release queue struct (unk00[2], unk08[2], count10, count11, unk12, unk13).
 *
 * @par Control flow
 * Two sequential for-loops over count10 and count11 calling func_80029650 on each handle, followed by field resets.
 *
 * @par Calls
 * func_80029650 - decrements the asset node reference count and unlinks/releases any zero-count nodes in D_800A0B70.
 *
 * @par Side effects
 * Resets D_800A0C30.count10, count11, unk12 to 0, unk13 to 0xFF, and decrements/releases assets in D_800A0B70.
 *
 * @par Callers
 * Confirmed: func_80029C34 (0x80029D4C, stream request queue step), func_80029EB8 (0x80029EE4, asset registry teardown).
 */
void func_80029520(void)
{
    u32 i;

    for (i = 0; i < gReleaseQueues.count10; i++) {
        func_80029650(gReleaseQueues.unk00[i]);
    }

    gReleaseQueues.count10 = 0;

    for (i = 0; i < gReleaseQueues.count11; i++) {
        func_80029650(gReleaseQueues.unk08[i]);
    }

    gReleaseQueues.count11 = 0;
    gReleaseQueues.unk12 = 0;
    gReleaseQueues.unk13 = 0xFF;
}
