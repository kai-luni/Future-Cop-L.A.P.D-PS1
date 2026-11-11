#include "game/types.h"

typedef struct {
    s8 unk00;
    s8 unk01;
    u8 unk02;
    u8 pad03;
    s32 unk04;
    s32 unk08;
    u8 pad0C[4];
} Unk800A0A00;

extern u8 D_8009F26C;
extern s32 D_8009F278;
extern s32 D_8009F27C;
extern u8 D_80023314[];
extern Unk800A0A00 D_800A0A00[8];

extern void func_80098708(s32 arg0);
extern s32 func_8001F12C(s32 callback);
extern void func_80021250(void);
extern void func_80023BF8(void);
extern void func_80021C80(void);
extern void func_80044FEC(void);

void func_80023478(void)
{
    if (D_8009F26C != 0) {
        if (D_8009F27C != 0) {
            func_80098708(D_8009F27C);
            D_8009F27C = 0;
        }
        if (D_8009F278 != 0) {
            func_80098708(D_8009F278);
            D_8009F278 = 0;
        }
        func_8001F12C((s32)D_80023314);
        func_80021250();
        func_80023BF8();
        func_80021C80();
        func_80044FEC();
        D_8009F26C = 0;
    }
}
