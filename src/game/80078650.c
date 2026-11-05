#include "game/types.h"

typedef struct Menu {
    u8 pad00[3];
    s8 unk03;
    u8 unk04;
    u8 unk05;
    u8 pad06[0xA];
    void (*unk10)(void);
    u8 pad14[0x10];
    struct Menu *unk24;
} Menu;

extern Menu *D_8009F468;
extern u8 D_8009F875;
extern u8 D_8009F876;
extern u8 D_8009F8C0;
extern u8 D_8009F8C1;
extern u8 D_8009F8CC;
extern void func_800785F8(void);
extern void func_800787D8(s32 a, s32 b, s32 c);
extern void func_800788B4(void);
extern void D_8007B3F4(void);

void func_80078650(Menu *arg0, s32 arg1)
{
    Menu *m;

    if (arg1 != 0) {
        arg0->unk24 = D_8009F468;
    }
    D_8009F468 = arg0;
    if (arg1 != 0) {
        func_800788B4();
    }
    D_8009F8C0 = 0;
    D_8009F8C1 = 0;
    func_800785F8();
    D_8009F8CC = 0;
    if (D_8009F468->unk10 != 0) {
        D_8009F468->unk10();
    }
    func_800787D8(0, 1, 0);
    if ((u8)(D_8009F468->unk05 - 2) < 2) {
        func_800787D8(0, 1, 1);
    }
    if ((void *)D_8009F468->unk10 == (void *)D_8007B3F4) {
        D_8009F468->unk10();
    }
    m = D_8009F468;
    if (m->unk03 != -1) {
        D_8009F875 = m->unk03;
        D_8009F876 = m->unk04;
    }
}
