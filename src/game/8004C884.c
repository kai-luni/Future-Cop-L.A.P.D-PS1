// Categories: Gameplay > Object Spawning | Resources > Spawn Tables
#include "game/types.h"

struct Unk4C884 {
    s32 words[0x13];
};

extern u8 func_8003C2A4(struct Unk4C884 *data);
extern void *func_80017EBC(s32 size, s32 tag);
extern void func_8003C404(struct Unk4C884 *data);

/**
 * @brief Clones and registers a 0x4C-byte spawn record for chunk type 0x5A.
 *
 * Validates deferred placement and slot occupancy via func_8003C2A4, allocates a
 * 0x4C-byte tag-2 block, copies all 19 words from source, and registers it via func_8003C404.
 *
 * @param source Pointer to the 0x4C-byte Unk4C884 spawn record (chunk type 0x5A).
 *
 * @par Globals
 * None directly; callees access D_8009F310 and D_800A7040.
 *
 * @par Control flow
 * Returns early if func_8003C2A4(source) != 0 or func_80017EBC(0x4C, 2) returns null; otherwise copies *source and calls func_8003C404.
 *
 * @par Calls
 * func_8003C2A4 - handles flags bit 0 deferred placement and checks active slot occupancy; func_80017EBC - allocates 0x4C tag-2 bytes; func_8003C404 - clears map cell and stores copy in D_800A7040.
 *
 * @par Side effects
 * Allocates a 0x4C-byte heap block and updates D_800A7040 and map state via callees.
 *
 * @par Callers
 * Confirmed: func_8003BFD0 (0x8003C080 via jtbl_800115B8 index 4, type 0x5A), func_8004CA98 (0x8004CAC0 on tACT chunk).
 */
void func_8004C884(struct Unk4C884 *source)
{
    struct Unk4C884 *copy;

    if (func_8003C2A4(source) != 0) {
        return;
    }
    copy = func_80017EBC(0x4C, 2);
    if (copy == 0) {
        return;
    }
    *copy = *source;
    func_8003C404(copy);
}
