// Categories: Resources > Spawn Tables | Gameplay > Object Spawning
#include "game/types.h"

struct Unk6EAF0 {
    s32 words[0x15];
};

extern u8 func_8003C2A4(struct Unk6EAF0 *data);
extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void gInstallSpawnRecord(struct Unk6EAF0 *data) __asm__("func_8003C404");
void gRegisterActorDef5C(struct Unk6EAF0 *source) __asm__("func_8006EAF0");


/**
 * @brief Clones and registers a 0x54-byte spawn-table descriptor into D_800A7040 unless already active.
 *
 * Checks deferred spawning and slot occupancy via func_8003C2A4; if it returns `0`, allocates a `0x54`-byte heap block in pool tag 2 via func_80017EBC, returns early if null, copies `*source`, and registers the copy via func_8003C404. Category Resources > Spawn Tables | Gameplay > Object Spawning follows D_800A7040 registration and callers func_8003BFD0/func_8006ED78.
 *
 * @param source Pointer to the `0x54`-byte (`0x15` s32 words) source spawn descriptor.
 *
 * @par Globals
 * None directly (callees read/write D_8009F310 and D_800A7040).
 *
 * @par Control flow
 * Early exits on `func_8003C2A4(source) != 0` or `copy == 0`, followed by 0x54-byte struct copy and func_8003C404.
 *
 * @par Calls
 * func_8003C2A4 - handles deferred placement when flag bit 0 is set and checks D_800A7040 occupancy.
 * func_80017EBC - allocates `0x54` bytes under heap tag 2; func_8003C404 - installs `copy` into D_800A7040.
 *
 * @par Side effects
 * Allocates 0x54 bytes on heap tag 2 and updates D_800A7040 (plus D_8009F30C when func_8003C2A4 queues).
 *
 * @par Callers
 * Confirmed: func_8003BFD0 (0x8003C070 in asm/us/main.s, case `0x59`) and func_8006ED78 (0x8006EDA0 in asm/us/main.s) / Likely: none / Unknown: none.
 *
 * @par Environment path
 * Registers one 0x54-byte actor definition of type 0x5C: func_8003C2A4 first
 * queues a deferred placement for records with flag bit 0 (+0x32) and returns
 * 1 only when the slot D_800A7040[D_8009F310][record +0x14] already holds a
 * bit-0 record, in which case nothing else happens; otherwise the record is
 * copied into a new tag-2 heap block and installed with gInstallSpawnRecord
 * (func_8003C404). It is called by the 'Cact' chunk loader func_8006ED78
 * (type 0x5C in func_8004D6D4, 8004D6D4.c:126) and by the 'Csfx' stream
 * walker func_8003BFD0, whose jump table jtbl_800115B8 maps the same type
 * 0x5C to this function (index 6); both paths therefore agree on the type
 * number. This is part of level spawn-table construction.
 */
void gRegisterActorDef5C(struct Unk6EAF0 *source)
{
    struct Unk6EAF0 *copy;

    if (func_8003C2A4(source) != 0) {
        return;
    }
    copy = gAllocMemory(0x54, 2);
    if (copy == 0) {
        return;
    }
    *copy = *source;
    gInstallSpawnRecord(copy);
}
