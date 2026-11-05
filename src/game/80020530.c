#include "game/types.h"

struct CardContext {
    u8 unk000[0x8C4];
    s32 lockCount;
    u8 unk8C8[4];
    s32 savedCallback;
};

extern struct CardContext *D_8009AB08;
extern u8 D_80020024[];

extern void func_8008C9C0(void);
extern s32 func_80091510(void *argument);
extern void func_8008C9E0(void);

void func_80020530(void)
{
    if (D_8009AB08->lockCount == 0) {
        func_8008C9C0();
        D_8009AB08->savedCallback = func_80091510(D_80020024);
        func_8008C9E0();
    }
    D_8009AB08->lockCount++;
}
