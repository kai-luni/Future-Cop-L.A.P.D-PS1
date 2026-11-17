// Categories: Graphics > Effects | Game Engine > World Reset
#include "game/types.h"

struct Pool {
    void *storage;
    void *active;
    void *free;
};

extern struct Pool gOwnerNodeListA __asm__("D_800A22A0");
extern struct Pool gOwnerNodeListB __asm__("D_800A22B0");
extern void *gDomeEffectBlock[] __asm__("D_800A22F0");

extern void func_8003BDEC(struct Pool *pool);
extern void func_8005E074(void *effect);

/**
 * @brief Tears down the two effect object pools and any active attached model-effect blocks.
 *
 * Releases the backing storage for pools D_800A22A0 (0x38-byte nodes) and D_800A22B0
 * (0x60-byte nodes), then frees every non-null slot in the 4-entry attached-effect array D_800A22F0.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_800A22A0/D_800A22B0 - effect/sprite Pool descriptors whose storage pointers are freed and cleared;
 * D_800A22F0 - 4-slot array of attached-effect blocks passed to func_8005E074 when non-null.
 * @par Control flow
 * Two unconditional pool resets followed by a 4-iteration for loop checking D_800A22F0[i] != 0.
 * @par Calls
 * func_8003BDEC - frees pool->storage and nulls it for D_800A22A0 and D_800A22B0;
 * func_8005E074 - unbinds each attached effect's model (func_8006CA68) and frees the block (func_800181D4).
 * @par Side effects
 * Frees pool storage and attached effect blocks; leaves D_800A22F0[i] slots non-null after freeing.
 * @par Callers
 * Confirmed: func_8003F854 (world teardown).
 */
void func_8005E22C(void)
{
    s32 i;

    func_8003BDEC(&gOwnerNodeListA);
    func_8003BDEC(&gOwnerNodeListB);
    for (i = 0; i < 4; i++) {
        if (gDomeEffectBlock[i] != 0) {
            func_8005E074(gDomeEffectBlock[i]);
        }
    }
}
