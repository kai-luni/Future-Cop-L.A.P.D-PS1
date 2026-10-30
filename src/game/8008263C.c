#include "game/types.h"

typedef struct {
    u8 unk00[0x1C];
    s32 unk1C;
} Unk8008263C;

extern Unk8008263C **D_8009F3F4;
extern void func_800181D4(void *block);

void func_8008263C(Unk8008263C *arg)
{
    if ((u32)(arg->unk1C - 1) < 0x1D) {
        if (D_8009F3F4 == 0) {
            goto end;
        }
        if (D_8009F3F4[arg->unk1C] != 0) {
            goto call;
        }
        D_8009F3F4[arg->unk1C] = arg;
        goto end;
    }
call:
    func_800181D4(arg);
end:
    ;
}
