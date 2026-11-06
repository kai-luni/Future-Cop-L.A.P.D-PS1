#include "game/types.h"

struct Control {
    u8 unk00[0x84];
    s16 unk84[3];
    s16 unk8A[3];
    u8 unk90[3];
};

struct Object {
    u8 unk00[0xAC];
    struct Control *ctl;
};

struct Target;
extern struct Target *func_8006AAEC(u8 id);
extern s32 func_8006BD48(struct Object *obj, struct Target *tgt, s32 flag);

s32 func_8006BE90(struct Object *obj, s32 idx, s32 step)
{
    struct Control *ctl = obj->ctl;
    struct Target *tgt = func_8006AAEC(ctl->unk90[idx]);
    s32 max = func_8006BD48(obj, tgt, 0);

    if (ctl->unk84[idx] < max) {
        if (step >= 0) {
            ctl->unk84[idx] += step;
            if (ctl->unk84[idx] > max) {
                ctl->unk84[idx] = max;
            }
        } else {
            ctl->unk84[idx] = max;
        }
        return 0;
    }
    return 1;
}
