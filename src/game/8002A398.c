#include "game/types.h"

typedef struct Unk8002A398 {
    u8 unk00[0x14];
    u16 unk14;
    u16 unk16;
} Unk8002A398;

extern u8 D_8009F308;
extern s32 D_8009F6CC;
extern u32 D_8009F6E4;
extern Unk8002A398 *D_800A0C50[4];

s32 func_8002A398(void)
{
    Unk8002A398 *ptr;

    ptr = 0;
    if (D_8009F308 != 1) {
        ptr = D_800A0C50[D_8009F6CC];
        if (ptr != 0) {
            D_8009F6E4 = ptr->unk16 + 1;
            if (D_8009F6E4 >= ptr->unk14) {
                D_8009F6E4 = 0;
            }
            D_800A0C50[D_8009F6CC] = 0;
            D_8009F6CC++;
            if (D_8009F6CC >= 4) {
                D_8009F6CC = 0;
            }
        }
    }
    return (s32)ptr;
}
