#include "game/types.h"

typedef void (*SpuIrqCallback)(void);

typedef struct {
    u8 pad[0x7B10];
    void *unk7B10;
    SpuIrqCallback unk7B14;
    s32 unk7B18;
    volatile s32 unk7B1C;
    volatile s32 unk7B20;
    volatile s32 unk7B24;
    u8 pad2[0x20];
    volatile s32 unk7B48;
} Unk8009F4D8;

extern Unk8009F4D8 *D_8009F4D8;

extern void func_8008C9C0(void);
extern void func_800876EC(void);
extern void func_80098A78(s32 arg);
extern SpuIrqCallback func_80098BF8(SpuIrqCallback callback);
extern void func_8008C9E0(void);
extern void func_80098FB8(s32 value);
extern void func_80091510(void *argument);
extern void func_80087688(void);

void func_80087750(void)
{
    func_8008C9C0();
    func_800876EC();
    func_80098A78(0);

    func_80098BF8(D_8009F4D8->unk7B14);
    func_8008C9E0();

    while (D_8009F4D8->unk7B48 != 0) {
    }

    func_8008C9C0();
    func_80098FB8(D_8009F4D8->unk7B18);
    D_8009F4D8->unk7B20 = 0;
    if (D_8009F4D8->unk7B1C != 0) {
        func_80091510(D_8009F4D8->unk7B10);
    }
    D_8009F4D8->unk7B24 = 0;
    func_8008C9E0();
    func_80087688();
}
