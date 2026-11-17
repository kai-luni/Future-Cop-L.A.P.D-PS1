// Categories: Graphics > Scene Graph > Node Pool | Platform > Sound | Resources > Memory Pools
#include "game/types.h"

extern s32 *gMissionTable __asm__("D_8009F930");
extern void *gMissionState __asm__("D_8009F934");
extern s32 gBucketArray __asm__("D_8009F938");
extern s32 gMissionClearFlag __asm__("D_8009F944");

extern void *func_80017EBC(s32 size, s32 tag);
extern void *func_800194DC(s32 count, s32 size, s32 tag);

/**
 * @brief Resets the scene render pools: the 250-node spawn pool, the 32-slot
 *        emitter table and the bucket-array fields.
 *
 * Part of the mission boot chain of func_80024A24. D_8009F934 receives the state
 * of a fixed pool of 0xFA blocks of 0x5C bytes - exactly the size of the scene
 * node struct - and D_8009F930 a 0x104-byte table whose first word is a 32-bit
 * "free" mask initialised to all ones. The bucket pointer and one counter are
 * cleared. The matching teardown is func_80083428.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Globals
 * D_8009F930 - emitter table {u32 free mask; 32 slots of {obj, s16 timer, s8
 * voice}}, used by func_8008363C to hand a scene node a sound voice.
 * D_8009F934 - pool state consumed by func_800195A4 in the node spawner
 * func_80083A10.
 * D_8009F938 - bucket-array pointer, nulled here, indexed by func_80083A10 and
 * freed by func_80083428; no unit in the decompiled set allocates it.
 * D_8009F944 - cleared here and written nowhere else in the package.
 *
 * @par Control flow
 * Two stores, two allocations, one more store of -1. No loops or branches.
 *
 * @par Calls
 * func_800194DC - creates the fixed-block pool (0xFA blocks, 0x5C bytes, tag 2).
 * func_80017EBC - plain tagged allocation of the 0x104-byte emitter table.
 *
 * @par Side effects
 * Writes the four globals, allocates a pool and a table, and sets the table's
 * free mask to all ones (every voice free).
 *
 * @par Callers
 * Confirmed: func_80024A24 (mission boot).
 */
void func_8008347C(void)
{
    s32 *p;

    gMissionClearFlag = 0;
    gBucketArray = 0;
    gMissionState = func_800194DC(0xFA, 0x5C, 2);
    p = func_80017EBC(0x104, 2);
    gMissionTable = p;
    *p = -1;
}
