// Categories: Graphics > Scene Graph | Platform > Sound | Resources > Memory Pools
#include "game/types.h"

extern s32 gMissionState __asm__("D_8009F934");
extern void *gBucketArray __asm__("D_8009F938");
extern void *gMissionTable __asm__("D_8009F930");

extern void gFreeBlock(void *block) __asm__("func_800181D4");
extern void func_80019584(s32 pool);

/**
 * @brief Frees the scene-node pool D_8009F934, bucket array D_8009F938, and emitter table D_8009F930.
 *
 * Confirmed: frees D_8009F934 (0xFA x 0x5C-byte pool allocated in func_8008347C) via func_80019584, frees non-null D_8009F938 (8-byte bucket grid) and D_8009F930 (0x104-byte 32-slot sound emitter table) via func_800181D4, and clears D_8009F934 and D_8009F938 (leaving D_8009F930 unzeroed).
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F934 - scene-node FixedPool freed and zeroed; D_8009F938 - bucket grid freed and zeroed; D_8009F930 - 32-slot emitter table freed when non-null.
 *
 * @par Control flow
 * Linear teardown with unconditional free of D_8009F934 and non-null checks before freeing D_8009F938 and D_8009F930.
 *
 * @par Calls
 * func_80019584 - forwards D_8009F934 to func_800181D4; func_800181D4 - frees D_8009F938 and D_8009F930.
 *
 * @par Side effects
 * Releases up to three heap allocations and clears D_8009F934 and D_8009F938.
 *
 * @par Callers
 * Confirmed: func_80024D14 (0x80024D88 in asm/us/main.s, global subsystem reset) / Likely: none / Unknown: none.
 */
void func_80083428(void)
{
    func_80019584(gMissionState);
    gMissionState = 0;
    if (gBucketArray != 0) {
        gFreeBlock(gBucketArray);
    }
    gBucketArray = 0;
    if (gMissionTable != 0) {
        gFreeBlock(gMissionTable);
    }
}
