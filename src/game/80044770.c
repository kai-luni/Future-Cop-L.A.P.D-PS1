// Categories: Resources > Spawn Tables | Gameplay > Object Spawning
#include "game/types.h"

struct Unk44770 {
    s32 words[0x18];
};

extern u8 func_8003C2A4(struct Unk44770 *data);
extern void *func_80017EBC(s32 size, s32 tag);
extern void func_8003C404(struct Unk44770 *data);

/**
 * @brief Clones and registers a 0x60-byte spawn-table descriptor into D_800A7040 unless already active.
 *
 * Runs the deferred-spawn / occupancy check via func_8003C2A4; if it returns `0`, allocates a `0x60`-byte heap block in pool tag 2 via func_80017EBC, copies `*source` into it without a null check, and publishes it via func_8003C404. Category Resources > Spawn Tables | Gameplay > Object Spawning follows D_800A7040 registration and callers func_8003BFD0/func_8004488C.
 *
 * @param source Pointer to the `0x60`-byte (`0x18` s32 words) source spawn descriptor.
 *
 * @par Globals
 * None directly (callees read/write D_8009F310 and D_800A7040).
 *
 * @par Control flow
 * Early return if `func_8003C2A4(source) != 0`; otherwise unconditional allocate, 0x60-byte copy, and register.
 *
 * @par Calls
 * func_8003C2A4 - handles deferred placement when flag bit 0 is set and checks D_800A7040 slot occupancy.
 * func_80017EBC - allocates `0x60` bytes under heap tag 2.
 * func_8003C404 - frees any prior D_800A7040 entry at `(D_8009F310, copy->unk14)` and installs `copy`.
 *
 * @par Side effects
 * Allocates 0x60 bytes on heap tag 2 and updates D_800A7040 (plus D_8009F30C when func_8003C2A4 queues).
 *
 * @par Callers
 * Confirmed: func_8003BFD0 (0x8003C050 in asm/us/main.s, case `0x57`) and func_8004488C (0x800448B4 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_80044770(struct Unk44770 *source)
{
    struct Unk44770 *copy;

    if (func_8003C2A4(source) != 0) {
        return;
    }
    copy = func_80017EBC(0x60, 2);
    *copy = *source;
    func_8003C404(copy);
}
