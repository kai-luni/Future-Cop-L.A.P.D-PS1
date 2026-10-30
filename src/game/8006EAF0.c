#include "game/types.h"

struct Unk6EAF0 {
    s32 words[0x15];
};

extern u8 func_8003C2A4(struct Unk6EAF0 *data);
extern void *func_80017EBC(s32 size, s32 tag);
extern void func_8003C404(struct Unk6EAF0 *data);

void func_8006EAF0(struct Unk6EAF0 *source)
{
    struct Unk6EAF0 *copy;

    if (func_8003C2A4(source) != 0) {
        return;
    }
    copy = func_80017EBC(0x54, 2);
    if (copy == 0) {
        return;
    }
    *copy = *source;
    func_8003C404(copy);
}
