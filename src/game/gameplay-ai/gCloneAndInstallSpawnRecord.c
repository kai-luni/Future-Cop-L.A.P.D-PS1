// Categories: Gameplay > Object Spawning | Resources > Spawn Tables
#include "game/types.h"

struct Unk767E4 {
    s32 words[0x16];
};

extern u8 gQueueDeferredGroundSpawn(struct Unk767E4 *data) __asm__("func_8003C2A4");
extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void gInstallSpawnRecord(struct Unk767E4 *data) __asm__("func_8003C404");
void gCloneAndInstallSpawnRecord(struct Unk767E4 *source) __asm__("func_800767E4");


/**
 * @brief Clones and registers a 0x58-byte spawn record for chunk type 0x57.
 *
 * Checks placement and slot occupancy via func_8003C2A4, allocates a 0x58-byte
 * tag-2 heap block, copies all 22 words from source, and installs it via func_8003C404.
 *
 * @param source Pointer to the 0x58-byte Unk767E4 spawn record (chunk type 0x57).
 *
 * @par Globals
 * None directly; callees access D_8009F310 and D_800A7040.
 *
 * @par Control flow
 * Returns early if func_8003C2A4(source) != 0 or func_80017EBC(0x58, 2) returns null; otherwise copies *source and invokes func_8003C404.
 *
 * @par Calls
 * func_8003C2A4 - queues deferred spawn on flags bit 0 and tests slot occupancy; func_80017EBC - allocates 0x58 bytes (mode 2); func_8003C404 - registers the copy in D_800A7040.
 *
 * @par Side effects
 * Allocates a 0x58-byte heap block and updates D_800A7040 and map grid state via callees.
 *
 * @par Callers
 * Confirmed: func_8003BFD0 (0x8003C0C0 via jtbl_800115B8 index 1, type 0x57), func_80076878 (0x800768A0 on tACT chunk).
 */
void gCloneAndInstallSpawnRecord(struct Unk767E4 *source)
{
    struct Unk767E4 *copy;

    if (gQueueDeferredGroundSpawn(source) != 0) {
        return;
    }
    copy = gAllocMemory(0x58, 2);
    if (copy == 0) {
        return;
    }
    *copy = *source;
    gInstallSpawnRecord(copy);
}
