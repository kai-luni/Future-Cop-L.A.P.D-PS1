#include "game/types.h"

extern u32 D_8009F4DC;
extern void *D_8009F4E4;
extern void *D_8009F4E8;
extern void *D_8009F4EC;
extern void *D_8009F4F0;
extern void *D_8009F4F4;
extern void *D_8009F4F8;
extern void *D_8009F4FC;
extern void *D_8009F500;
extern u8 D_8009F504;

extern void func_80085A24(void);
extern void func_80085AC4(s32, s32);
extern s32 func_800858E0(void);
extern void func_8001C088(u8, u8);
extern void func_80091270(s32);
extern s32 func_8001BFE4(void);
extern void func_80098708(void *);
extern void func_800879BC(void *);

void func_80087D48(void)
{
    func_80085A24();

    if (!(D_8009F4DC & 0x20)) {
        func_80085AC4(1, 0);
        func_8001C088((u8)func_800858E0(), 0);
        func_80091270(0);
        func_8001BFE4();
    }

    if (D_8009F4F8 != 0) {
        func_80098708(D_8009F4F8);
    }
    if (D_8009F4F4 != 0) {
        func_80098708(D_8009F4F4);
    }
    if (D_8009F4F0 != 0) {
        func_800879BC(D_8009F4F0);
    }
    if (!(D_8009F4DC & 1)) {
        if (D_8009F4EC != 0) {
            func_800879BC(D_8009F4EC);
        }
    }
    if (D_8009F4E8 != 0) {
        func_800879BC(D_8009F4E8);
    }
    if (D_8009F500 != 0) {
        func_800879BC(D_8009F500);
    }
    if (D_8009F4FC != 0) {
        func_800879BC(D_8009F4FC);
    }
    if (D_8009F4E4 != 0) {
        func_800879BC(D_8009F4E4);
    }

    D_8009F4E4 = 0;
    D_8009F4E8 = 0;
    D_8009F4EC = 0;
    D_8009F4F0 = 0;
    D_8009F4F4 = 0;
    D_8009F4F8 = 0;
    D_8009F4FC = 0;
    D_8009F500 = 0;
    D_8009F504 = 0;
}
