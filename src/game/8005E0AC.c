// Categories: Graphics > Effects > Dispatcher | Game Engine > Frame
#include "game/types.h"

struct Pool {
    void *storage;
    void *active;
    void *free;
};

extern struct Pool gOwnerNodeListA __asm__("D_800A22A0");
extern struct Pool gOwnerNodeListB __asm__("D_800A22B0");
extern void *gDomeEffectBlock[] __asm__("D_800A22F0");

extern void func_8005B3D4(void *object);
extern void func_8005BD20(struct Pool *pool, void *object);
extern void func_8005C0C0(struct Pool *pool, void *object);
extern u8 func_8005D8F4(void *effect, void *object);
extern void func_8005E074(void *effect);

/**
 * @brief Per-frame effect driver: beam timer, both pool sweeps and the four
 *        active attached effects.
 *
 * @param object Frame delta forwarded to the beam timer.
 * @return Nothing.
 *
 * @par Confirmed
 * func_8005B3D4(object) ticks the locked-target beam, func_8005BD20 and
 * func_8005C0C0 sweep the two pools (D_800A22A0 effects, D_800A22B0 sprites).
 * Then for each of the four slots D_800A22F0[i]: when occupied and
 * func_8005D8F4(effect, object) returns anything but 1, func_8005E074
 * releases the effect and the slot is cleared.
 *
 * @par Likely role
 * The effect subsystem's frame hook, called by func_8003F570: the return
 * value 1 of func_8005D8F4 means "still alive", everything else expires the
 * attached effect.
 *
 * @par Callers
 * func_8003F570.
 */
void func_8005E0AC(void *object)
{
    s32 i;

    func_8005B3D4(object);
    func_8005BD20(&gOwnerNodeListA, object);
    func_8005C0C0(&gOwnerNodeListB, object);
    for (i = 0; i < 4; i++) {
        if (gDomeEffectBlock[i] != 0 && func_8005D8F4(gDomeEffectBlock[i], object) != 1) {
            func_8005E074(gDomeEffectBlock[i]);
            gDomeEffectBlock[i] = 0;
        }
    }
}
