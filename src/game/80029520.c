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

extern void gReleaseStreamAssetReference(s32 arg0) __asm__("func_80029650");

void gFlushDeferredAssetReleases(void) __asm__("func_80029520");

/**
 * @brief Releases the handles from both deferred asset queues and resets their state bytes.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Walks the handle array at +0x00 using count +0x10, zeroes that count, then walks the array at
 * +0x08 using count +0x11. Calls gReleaseStreamAssetReference for each handle and writes 0, 0
 * and 0xFF at offsets +0x11 through +0x13.
 *
 * @par Globals
 * Reads and resets gReleaseQueues. The release helper may recycle buffers in gStreamBufferList.
 *
 * @par Callers
 * Direct calls from func_80029C34, func_80029EB8.
 *
 * @par Role and path
 * The stream request transition and asset registry teardown flush these retained references.
 *
 * @par Unknown
 * The arrays are used for different chunk kinds, but this flush applies the same release
 * operation to both.
 */
void gFlushDeferredAssetReleases(void)
{
    u32 i;

    for (i = 0; i < gReleaseQueues.count10; i++) {
        gReleaseStreamAssetReference(gReleaseQueues.unk00[i]);
    }

    gReleaseQueues.count10 = 0;

    for (i = 0; i < gReleaseQueues.count11; i++) {
        gReleaseStreamAssetReference(gReleaseQueues.unk08[i]);
    }

    gReleaseQueues.count11 = 0;
    gReleaseQueues.unk12 = 0;
    gReleaseQueues.unk13 = 0xFF;
}
