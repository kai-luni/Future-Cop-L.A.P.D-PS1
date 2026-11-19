// Categories: Gameplay > Object Spawning | Resources > Spawn Tables
#include "game/types.h"

struct Unk703A8 {
    s32 words[0x16];
};

extern u8 func_8003C2A4(struct Unk703A8 *data);
extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void gInstallSpawnRecord(struct Unk703A8 *data) __asm__("func_8003C404");
void gRegisterActorDef59(struct Unk703A8 *source) __asm__("func_800703A8");


/**
 * @brief Clones and registers a 0x58-byte spawn record if not already deferred.
 *
 * Checks placement and table occupancy via func_8003C2A4, allocates a 0x58-byte
 * tag-2 block, copies all 22 s32 words from source, and registers it via func_8003C404.
 *
 * @param source Pointer to the 0x58-byte Unk703A8 spawn record (chunk type 0x59).
 *
 * @par Globals
 * None directly; callees access D_8009F310 and D_800A7040.
 *
 * @par Control flow
 * Returns early if func_8003C2A4(source) is non-zero or func_80017EBC returns null; otherwise copies *source and calls func_8003C404.
 *
 * @par Calls
 * func_8003C2A4 - handles flags bit 0 deferred placement and checks active slot occupancy; func_80017EBC - allocates 0x58 tag-2 bytes; func_8003C404 - clears map cell and stores copy in D_800A7040.
 *
 * @par Side effects
 * Allocates a 0x58-byte heap block and updates D_800A7040 and map state via callees.
 *
 * @par Callers
 * Confirmed: func_8003BFD0 (0x8003C0A0 via jtbl_800115B8 index 3, type 0x59), func_800706C4 (0x800706EC on tACT chunk).
 *
 * @par Environment path
 * Registers one 0x58-byte actor definition of type 0x59: func_8003C2A4 first
 * queues a deferred placement for records with flag bit 0 (+0x32) and returns
 * 1 only when the slot D_800A7040[D_8009F310][record +0x14] already holds a
 * bit-0 record, in which case nothing else happens; otherwise the record is
 * copied into a new tag-2 heap block and installed with gInstallSpawnRecord
 * (func_8003C404). It is called by the 'Cact' chunk loader func_800706C4
 * (type 0x59 in func_8004D6D4, 8004D6D4.c:135) and by the 'Csfx' stream
 * walker func_8003BFD0, whose jump table jtbl_800115B8 maps the same type
 * 0x59 to this function (index 3); both paths therefore agree on the type
 * number. Instances of this type are later built from the stored record by
 * func_8007043C (case 0x59 of func_8003E984), which reads record fields
 * +0x34..+0x56 and installs the message handler func_8006FF44.
 */
void gRegisterActorDef59(struct Unk703A8 *source)
{
    struct Unk703A8 *copy;

    if (func_8003C2A4(source) != 0) {
        return;
    }
    copy = gAllocMemory(0x58, 2);
    if (copy == 0) {
        return;
    }
    *copy = *source;
    gInstallSpawnRecord(copy);
}
