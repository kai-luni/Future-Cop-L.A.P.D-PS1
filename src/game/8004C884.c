// Categories: Gameplay > Object Spawning | Resources > Spawn Tables
#include "game/types.h"

struct Unk4C884 {
    s32 words[0x13];
};

extern u8 func_8003C2A4(struct Unk4C884 *data);
extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void gInstallSpawnRecord(struct Unk4C884 *data) __asm__("func_8003C404");
void gRegisterActorDef5A(struct Unk4C884 *source) __asm__("func_8004C884");


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
 *
 * @par Environment path
 * Registers one 0x4C-byte actor definition of type 0x5A: func_8003C2A4 first
 * queues a deferred placement for records with flag bit 0 (+0x32) and returns
 * 1 only when the slot D_800A7040[D_8009F310][record +0x14] already holds a
 * bit-0 record, in which case nothing else happens; otherwise the record is
 * copied into a new tag-2 heap block and installed with gInstallSpawnRecord
 * (func_8003C404). It is called by the 'Cact' chunk loader func_8004CA98
 * (type 0x5A in func_8004D6D4, 8004D6D4.c:129) and by the 'Csfx' stream
 * walker func_8003BFD0, whose jump table jtbl_800115B8 maps the same type
 * 0x5A to this function (index 4); both paths therefore agree on the type
 * number. This is part of level spawn-table construction.
 */
void gRegisterActorDef5A(struct Unk4C884 *source)
{
    struct Unk4C884 *copy;

    if (func_8003C2A4(source) != 0) {
        return;
    }
    copy = gAllocMemory(0x4C, 2);
    if (copy == 0) {
        return;
    }
    *copy = *source;
    gInstallSpawnRecord(copy);
}
