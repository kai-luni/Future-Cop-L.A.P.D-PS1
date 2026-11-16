// Categories: Physics > Collision > Radius Tests | Gameplay > Combat
#include "game/types.h"

struct Target;
struct Detail {
    u8 pad[0x14];
    u16 id;
};
struct Source {
    u8 pad00[8];
    struct Detail *detail;
    u8 pad0c[0x10];
    u16 value;
    u8 pad1e[0xc];
    u8 flags;
};
struct Target {
    u8 pad00[4];
    void (*callback)(s32, struct Target *, void *, s32);
    u8 pad08[0x54];
    s32 x, y, z;
    u8 pad68[6];
    s16 radius;
};
struct Sphere {
    u8 pad00[4];
    s32 x, y, z;
    s16 radius;
    u8 state;
};
struct Event {
    u32 id;
    u16 value;
    u8 type;
    u8 pad;
    struct Detail *detail;
};
extern s32 func_80019FCC(s32 *);

/**
 * @brief Tests 3D sphere-vs-target overlap and dispatches a hit event on collision.
 *
 * Computes the 3D offset between sphere and target, obtains its 16.16 squared distance
 * via func_80019FCC, and compares against ((sphere->radius + target->radius)^2) >> 8.
 * On overlap it calls target->callback with message 14 and marks the sphere consumed.
 *
 * @param source Effect source record (+0x08 detail/attacker, +0x1C value, +0x2C flags).
 * @param target Candidate target entity (+0x04 callback, +0x5C..+0x64 pos, +0x6E radius); ignored if null.
 * @param sphere Active collision sphere (+0x04..+0x0C pos, +0x10 radius, +0x12 state).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Guards on target != 0; tests distance < radius; on hit populates Event (type 2), calls target->callback(14, ...), sets sphere->state = 10, and ORs 8 into source->flags if bit 2 is set.
 *
 * @par Calls
 * func_80019FCC - computes 16.16-scaled squared 3D length; indirect call to target->callback (message 14).
 *
 * @par Side effects
 * Delivers hit message 14 to target, sets sphere->state = 10, and may set source->flags bit 3.
 *
 * @par Callers
 * Confirmed: func_8005B6A8 (0x8005B718 primary target, 0x8005B858 candidate table D_800A8F90).
 */
void func_8005B5A0(struct Source *source, struct Target *target, struct Sphere *sphere)
{
    s32 vec[3];
    struct Event event;
    register s32 radius __asm__("$4");
    s32 distance;
    if (target != 0) {
        vec[0] = sphere->x - target->x;
        vec[1] = sphere->y - target->y;
        vec[2] = sphere->z - target->z;
        distance = func_80019FCC(vec);
        radius = sphere->radius;
        radius += target->radius;
        radius = (radius * radius) >> 8;
        if (distance < radius) {
            event.type = 2;
            event.value = source->value;
            event.id = source->detail->id;
            event.detail = source->detail;
            target->callback(14, target, &event, 0);
            sphere->state = 10;
            if (source->flags & 4) {
                source->flags |= 8;
            }
        }
    }
}
