// Categories: Gameplay > Effect Pools
#include "game/types.h"

struct Pool {
    void *storage;
    void *active;
    void *free;
};

extern struct Pool gOwnerNodeListA __asm__("D_800A22A0");
extern struct Pool gOwnerNodeListB __asm__("D_800A22B0");
extern void *gDomeEffectBlock[] __asm__("D_800A22F0");
extern s16 D_800A22E8[];
extern s32 gSpawnMotionContext __asm__("D_8009F374");

extern void func_8003BE2C(struct Pool *pool, s32 size, s32 count);

/**
 * @brief Creates the two effect pools and clears the attached-effect slots.
 *
 * The pool for the 0x38-byte particle blocks and the pool for the 0x60-byte
 * effect blocks are both created with ten slots, the first free marker
 * D_800A22E8 is set to -1, all four attached-effect pointers D_800A22F0 are
 * cleared and the running counter D_8009F374 is reset.
 * @par Globals
 * D_800A22A0/D_800A22B0 - pools; D_800A22E8 - free marker;
 * D_800A22F0 - four attached-effect slots; D_8009F374 - counter.
 * @par Control flow
 * Two pool setups and one small clear loop.
 * @par Calls
 * func_8003BE2C - pool creation.
 * @par Side effects
 * Allocates both pools and resets the bookkeeping.
 * @par Callers
 * Confirmed: the level/world init path (func_8003F854 neighbourhood).
 */
void func_8005E1BC(void)
{
    s32 i;

    D_800A22E8[0] = -1;
    func_8003BE2C(&gOwnerNodeListA, 0x38, 0xA);
    func_8003BE2C(&gOwnerNodeListB, 0x60, 0xA);
    for (i = 3; i >= 0; i--) {
        gDomeEffectBlock[i] = 0;
    }
    gSpawnMotionContext = 0;
}
