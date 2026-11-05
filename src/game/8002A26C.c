#include "game/types.h"

extern u8 D_8009F6B8;
extern s32 D_8009F6BC;
extern s32 D_8009F6C4;
extern s32 D_8009F6DC;
extern s32 D_8009FA24;
extern s32 D_8009FA2C;
extern u8 D_800A0B90[];

extern void func_80029DD8(void);
extern void func_8002A16C(s32 arg0);
extern void func_8002A408(void);

void func_8002A26C(void)
{
    func_8002A408();

    if (D_8009F6C4 != 0 && D_8009F6C4 < D_8009FA2C) {
        D_8009F6C4 = 0;
        func_80029DD8();
        if (D_8009F6B8 & 0x40) {
            D_8009F6B8 = 0;
            D_8009F6BC = -1;
        }
        if (D_8009FA24 == 0) {
            if (D_8009F6DC == 2 || D_8009F6DC == 0) {
                D_800A0B90[0] = 1;
                func_8002A16C(1);
            }
        }
    }
}
