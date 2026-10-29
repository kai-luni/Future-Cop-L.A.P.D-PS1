#include "game/types.h"

extern void func_8008D6D0(s32);
extern void func_8008D670(s32, s32, s32);

void func_8008D690(s32 arg0) {
    func_8008D6D0(arg0);
    func_8008D670(arg0, 0x3F, 0);
}
