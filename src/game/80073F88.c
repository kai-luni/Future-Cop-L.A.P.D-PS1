// Categories: Gameplay > Object Spawning | Resources > Spawn Tables
#include "game/types.h"

struct Unk73F88 {
    s32 words[0x13];
};

extern u8 gQueueDeferredGroundSpawn(struct Unk73F88 *data) __asm__("func_8003C2A4");
extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void gInstallSpawnRecord(struct Unk73F88 *data) __asm__("func_8003C404");

/**
 * @brief Clones and registers a 0x4C-byte spawn record for chunk type 0x58.
 *
 * Validates deferred placement and slot occupancy via func_8003C2A4, allocates a
 * 0x4C-byte tag-2 block, copies all 19 s32 words from source, and installs it via func_8003C404.
 *
 * @param source Pointer to the 0x4C-byte Unk73F88 spawn record (chunk type 0x58).
 *
 * @par Globals
 * None directly; callees access D_8009F310 and D_800A7040.
 *
 * @par Control flow
 * Returns early if func_8003C2A4(source) != 0 or func_80017EBC(0x4C, 2) returns null; otherwise copies *source and invokes func_8003C404.
 *
 * @par Calls
 * func_8003C2A4 - handles flags bit 0 deferred placement and tests slot occupancy; func_80017EBC - allocates 0x4C tag-2 bytes; func_8003C404 - clears map cell and stores copy in D_800A7040.
 *
 * @par Side effects
 * Allocates a 0x4C-byte heap block and updates D_800A7040 and map state via callees.
 *
 * @par Callers
 * Confirmed: func_8003BFD0 (0x8003C0B0 via jtbl_800115B8 index 2, type 0x58), func_800740B8 (0x800740E0 on tACT chunk).
 */
void func_80073F88(struct Unk73F88 *source)
{
    struct Unk73F88 *copy;

    if (gQueueDeferredGroundSpawn(source) != 0) {
        return;
    }
    copy = gAllocMemory(0x4C, 2);
    if (copy == 0) {
        return;
    }
    *copy = *source;
    gInstallSpawnRecord(copy);
}
