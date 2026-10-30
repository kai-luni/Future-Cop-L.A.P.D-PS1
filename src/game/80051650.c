#include "game/types.h"

struct Unk51650 {
    u8 unk00[8];
    s32 unk08;
};

extern u8 func_80019AA8(struct Unk51650 *resource);
extern void func_80019A74(struct Unk51650 *resource);
extern void func_80019D00(struct Unk51650 *resource);

void func_80051650(struct Unk51650 *resource)
{
    if (func_80019AA8(resource) == 0) {
        resource->unk08 = 0;
        func_80019A74(resource);
    } else {
        func_80019D00(resource);
    }
}
