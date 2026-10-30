#include "game/types.h"

struct Unk51580Entry {
    s32 unk00;
    s32 unk04;
    s32 unk08;
};

struct Unk51580Header {
    s32 unk00;
    s32 unk04;
    u32 count;
};

struct Unk51580 {
    s32 unk00;
    u8 unk04[0x1C];
    s32 unk20;
};

struct Unk51580Out {
    s32 unk00;
    s32 unk04;
    s32 unk08;
};

extern struct Unk51580Header *func_80019A20(s32 magic, s32 arg1, s32 arg2);
extern void func_8001AB78(s32 arg0, struct Unk51580Out *out);

s32 func_80051580(struct Unk51580 *arg0, s32 arg1, struct Unk51580Out *arg2)
{
    struct Unk51580Out *out;
    register struct Unk51580Entry *entry __asm__("$3");

    out = arg2;
    if (arg0 != 0) {
        if (func_80019A20(0x74415048, arg0->unk00, arg0->unk20)->count >= (u32)arg1) {
            if (arg1 > 0) {
                entry = (struct Unk51580Entry *)func_80019A20(0x61415048, arg0->unk00, arg0->unk20)
                        + (arg1 - 1);
                out->unk00 = entry->unk04;
                out->unk04 = 0;
                out->unk08 = entry->unk00;
                func_8001AB78(entry->unk08, out);
                return 1;
            }
        }
    }
    out->unk00 = 0;
    out->unk04 = 0;
    out->unk08 = 0;
    return 0;
}
