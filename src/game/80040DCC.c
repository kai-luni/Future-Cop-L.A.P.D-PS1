// Categories: Resources > Spawn Tables | Gameplay > Object Spawning
#include "game/types.h"

struct Unk40DCC {
    s32 words[0x16];
};

extern u8 func_8003C2A4(struct Unk40DCC *data);
extern void *func_80017EBC(s32 size, s32 tag);
extern void func_8003C404(struct Unk40DCC *data);

/**
 * @brief Clones and registers a 0x58-byte spawn-table descriptor into D_800A7040 unless already active.
 *
 * Checks deferred spawning and slot occupancy via func_8003C2A4; if it returns `0`, allocates a `0x58`-byte heap block in pool tag 2 via func_80017EBC, returns early if null, copies `*source`, and registers the copy via func_8003C404. Category Resources > Spawn Tables | Gameplay > Object Spawning follows D_800A7040 registration and callers func_8003BFD0/func_80041070.
 *
 * @param source Pointer to the `0x58`-byte (`0x16` s32 words) source spawn descriptor.
 *
 * @par Globals
 * None directly (callees read/write D_8009F310 and D_800A7040).
 *
 * @par Control flow
 * Early exits on `func_8003C2A4(source) != 0` or `copy == 0`, followed by 0x58-byte struct copy and func_8003C404.
 *
 * @par Calls
 * func_8003C2A4 - handles deferred placement when flag bit 0 is set and checks D_800A7040 occupancy.
 * func_80017EBC - allocates `0x58` bytes under heap tag 2; func_8003C404 - installs `copy` into D_800A7040.
 *
 * @par Side effects
 * Allocates 0x58 bytes on heap tag 2 and updates D_800A7040 (plus D_8009F30C when func_8003C2A4 queues).
 *
 * @par Callers
 * Confirmed: func_8003BFD0 (0x8003C060 in asm/us/main.s, case `0x58`) and func_80041070 (0x80041098 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_80040DCC(struct Unk40DCC *source)
{
    struct Unk40DCC *copy;

    if (func_8003C2A4(source) != 0) {
        return;
    }
    copy = func_80017EBC(0x58, 2);
    if (copy == 0) {
        return;
    }
    *copy = *source;
    func_8003C404(copy);
}
