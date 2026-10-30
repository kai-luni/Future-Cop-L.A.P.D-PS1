#include "game/types.h"

typedef struct {
    u8 pad0[0x408];
    volatile s32 unk408;
    volatile s32 unk40C;
    volatile s32 unk410;
    u8 pad414[0x8];
    volatile s32 unk41C;
    volatile s32 unk420;
} Unk8009F970;

extern Unk8009F970 *D_8009F970;
extern s32 D_8009F510;
extern s32 D_8009F514;
extern u8 D_80088D74[];
extern u8 func_800889DC[];

extern s32 func_8008E154(s32 mode, u8 *result);
extern void func_8008E42C(s32 mode, u8 *buf, s32 arg2);
extern void func_8008C9C0(void);
extern void func_8008C9E0(void);
extern s32 func_8008E194(s32 value);
extern s32 func_8008E1A8(s32 value);

void func_800895E4(s32 arg0)
{
    u8 buf[8];

    if (arg0 != 0) {
        func_8008E154(0, 0);
        buf[0] = 0xA0;
        func_8008E42C(0xE, buf, 0);
        func_8008C9C0();
        D_8009F970->unk410 = 0;
        D_8009F970->unk408 = 0;
        D_8009F970->unk40C = 0;
        D_8009F510 = func_8008E194((s32)D_80088D74);
        D_8009F514 = func_8008E1A8((s32)(func_800889DC + 0x40));
        D_8009F970->unk420 = 1;
        D_8009F970->unk41C = 1;
    } else {
        s32 active;

        func_8008C9C0();
        active = D_8009F970->unk420;
        D_8009F970->unk41C = 0;
        D_8009F970->unk410 = 0;
        D_8009F970->unk408 = 0;
        D_8009F970->unk40C = 0;
        if (active != 0) {
            func_8008E194(D_8009F510);
            func_8008E1A8(D_8009F514);
            D_8009F510 = 0;
            D_8009F514 = 0;
            D_8009F970->unk420 = 0;
        }
    }
    func_8008C9E0();
}
