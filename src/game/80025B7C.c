#include "game/types.h"

typedef struct {
    u8 unk00[0x10];
    s32 unk10;
    u8 unk14[0x2C];
    s32 unk40;
    s32 unk44;
    u8 unk48[0x18];
    s32 unk60;
} UnkFA94;

extern UnkFA94 *D_8009FA94;

extern s32 D_8009F9C4;
extern s32 D_8009F9FC;
extern s32 D_8009FA00;
extern s32 D_8009FA30;
extern s32 D_8009FA70;
extern s32 D_8009FAA4;
extern void *jtbl_80010DDC[];

extern void func_80092C8C(s32 arg0);
extern void func_8001EB6C(void);
extern u8 func_80024170(void);
extern void func_80025348(void);
extern void func_80047690(u8 enable);
extern void func_80029DD8(void);
extern void func_80023BF8(void);
extern void func_800249BC(s32 arg0);
extern void func_80024D14(s32 arg0);

void func_80025B7C(s32 arg0)
{
    register s32 state __asm__("$16");
    register s32 saved_arg __asm__("$17");
    register u32 index __asm__("$3");
    static void *const label_table[] __attribute__((used)) = {
        &&state_5, &&state_6, &&state_7, &&state_8, &&state_9,
        &&state_default, &&state_default, &&state_default,
        &&state_default, &&state_default
    };

    saved_arg = arg0;
    func_80092C8C(0);
    func_8001EB6C();

    index = (u32)D_8009FA00 - 5;
    if (index >= 10) {
        goto state_default;
    }
    goto *jtbl_80010DDC[index];

state_5:
        D_8009FA94->unk10 = 2;
        goto state_done;
state_6:
        D_8009FA94->unk10 = 2;
        goto state_done;
state_7:
        D_8009FA94->unk10 = 3;
        goto state_done;
state_8:
        if (D_8009FA70 == 0) {
            D_8009FA00 = 10;
            state = D_8009FA00;
            while (1) {
                if (D_8009FA00 != state && func_80024170() == 0) {
                    break;
                }
                D_8009F9FC = D_8009FAA4 - D_8009FA30;
                D_8009FA30 = D_8009FAA4;
                func_80025348();
            }
        }
        D_8009FA94->unk10 = 0;
        goto state_done;
state_9:
        if (D_8009FA94->unk40 == D_8009FA94->unk44) {
            D_8009FA94->unk10 = 0;
        } else {
            D_8009FA94->unk10 = 1;
        }
        goto state_done;
state_default:
        D_8009FA94->unk10 = 4;

state_done:
    func_80047690(0);
    func_80029DD8();
    func_80023BF8();

    D_8009FA94->unk60 = 0;
    if (D_8009F9C4 != 0) {
        func_800249BC(saved_arg);
    } else {
        func_80024D14(saved_arg);
    }
}
