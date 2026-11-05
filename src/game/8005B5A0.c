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
