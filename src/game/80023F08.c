#include "game/types.h"

struct Resource { u8 pad[13]; s8 owner; };
struct Handle { struct Resource *resource; s32 serial, state, id, time, delay; u8 flag; };
struct Params { s32 pitch, volume, pan; };
extern struct Handle D_800A0A80;
extern s32 D_8009F278, D_8009FA2C;
extern void func_80023E84(void);
extern s32 func_800474C4(struct Handle *);
extern s32 func_800450A4(s32, s8 *);
extern s8 func_80045050(void);
extern s32 func_800220C4(s32, s32, struct Params *, s32);
extern s32 func_800474F0(struct Handle *);
extern void func_80047800(struct Resource *, s32, s32, s32);
extern void func_800221E4(s32, s32 *);

s32 func_80023F08(void)
{
    s8 mix[24];
    struct Params params;
    s32 channel;
    struct Resource *resource;

    if (!D_8009F278) func_80023E84();
    if ((u8)func_800474C4(&D_800A0A80) == 1) {
        resource = D_800A0A80.resource;
        if (D_800A0A80.flag) {
            if (!(u8)func_800450A4(D_800A0A80.id, mix)) {
                resource = 0;
                func_80023E84();
            }
        } else {
            mix[0] = func_80045050();
            mix[1] = 0x40;
        }
        if (resource) {
            params.pitch = 0x400;
            params.volume = mix[0];
            params.pan = mix[1];
            if (func_800220C4(100, resource->owner, &params, D_8009F278) == -1) {
                func_800474F0(&D_800A0A80);
            } else {
                if (D_800A0A80.flag) func_80047800(resource, mix[1], mix[0], -1);
                channel = resource->owner;
                func_800221E4(1, &channel);
                D_800A0A80.state = 1;
                D_800A0A80.time = D_8009FA2C + D_800A0A80.delay;
            }
        }
    }
    return D_800A0A80.state == 1;
}
