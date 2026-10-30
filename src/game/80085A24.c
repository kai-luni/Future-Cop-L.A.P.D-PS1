#include "game/types.h"

typedef void (*DecDctCallback)(void);

typedef struct {
    u8 pad[0x120];
    DecDctCallback unk120;
    DecDctCallback unk124;
    DecDctCallback unk128;
} Unk8009F4BC;

extern Unk8009F4BC *D_8009F4BC;

extern s32 func_80096090(s32 mode);
extern s32 func_800960CC(s32 mode);
extern void func_80092C8C(s32 arg);
extern void func_8008C9C0(void);
extern DecDctCallback func_8009612C(DecDctCallback callback);
extern DecDctCallback func_80096108(DecDctCallback callback);
extern void func_80092B94(DecDctCallback callback);
extern void func_8008C9E0(void);
extern void func_80095E90(s32 option);

void func_80085A24(void)
{
    if (D_8009F4BC != 0) {
        func_80096090(0);
        func_800960CC(0);
        func_80092C8C(0);
        func_8008C9C0();

        func_8009612C(D_8009F4BC->unk120);
        func_80096108(D_8009F4BC->unk124);
        func_80092B94(D_8009F4BC->unk128);

        D_8009F4BC->unk120 = 0;
        D_8009F4BC->unk124 = 0;
        D_8009F4BC->unk128 = 0;

        func_8008C9E0();
        func_80095E90(1);
        D_8009F4BC = 0;
    }
}
