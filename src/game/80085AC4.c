#include "game/types.h"

typedef struct {
    u8 pad[0x114];
    s32 unk114;
    s32 unk118;
} Ctx85AC4;

extern Ctx85AC4 *D_8009F4BC;
extern volatile u8 D_8009FA3C;
extern u32 D_8009F4C8;

extern void func_80092C8C(s32 arg0);
extern void func_80096090(s32 arg0);
extern void func_800960CC(s32 arg0);
extern void func_80092E10(s16 *buf, s32 a1, s32 a2, s32 a3);
extern void func_80091270(s32 arg0);
extern void func_800932F0(void *ptr);
extern void func_800934BC(void *ptr);
extern void func_80092BF4(s32 arg0);
extern void func_80095E90(s32 arg0);

void func_80085AC4(u32 arg0, u32 arg1)
{
    s16 buf[4];

    D_8009F4BC->unk114 = 0;
    D_8009F4BC->unk118 = 0;

    while (D_8009FA3C != 0) {
    }

    func_80092C8C(0);
    func_80096090(0);
    func_800960CC(0);

    if ((arg0 & 0xFF) != 0) {
        buf[0] = 0;
        buf[1] = 0;
        buf[3] = 0xF0;
        buf[2] = ((D_8009F4C8 * 5) << 6) >> 1;
        func_80092E10(buf, 0, 0, 0);
        func_80092C8C(0);

        buf[0] = 0;
        buf[1] = 0x100;
        buf[3] = 0xF0;
        buf[2] = ((D_8009F4C8 * 5) << 6) >> 1;
        func_80092E10(buf, 0, 0, 0);
        func_80092C8C(0);
    }

    if ((arg1 & 0xFF) != 0) {
        func_80091270(0);
        func_800932F0((u8 *)D_8009F4BC + 0x34 + (1 - D_8009F4BC->unk114) * 92);
        func_800934BC((u8 *)D_8009F4BC + 0xEC + (1 - D_8009F4BC->unk114) * 20);
        func_80092BF4(1);
    }

    func_80095E90(0);
}
