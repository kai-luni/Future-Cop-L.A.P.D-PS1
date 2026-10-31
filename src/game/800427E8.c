#include "game/types.h"

struct Unk427E8 {
    u8 unk00[0x6C];
    void *unk6C;
    u8 unk70[0x34];
    s16 unkA4;
    u8 unkA6[0x2];
    void *unkA8;
};

extern s32 D_8009FAB0;
extern void func_800181D4(void *block);

void func_800427E8(struct Unk427E8 *object)
{
    D_8009FAB0 -= object->unkA4;
    if (object->unkA8 != 0) {
        func_800181D4(object->unkA8);
    }
    func_800181D4(object->unk6C);
    func_800181D4(object);
}
