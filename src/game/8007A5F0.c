#include "game/types.h"

struct Unk78210Mode {
    u8 unk00[0x08];
    s32 unk08;
    u8 unk0C[0x50];
    s32 split;
    u8 unk60[0x38];
    s32 unk98;
};

struct Unk78210Panel {
    u8 unk00[5];
    u8 visible;
    u8 unk06[0x2A];
};

extern struct Unk78210Mode *D_8009FA94;
extern volatile s32 D_8009F8F0;
extern s32 D_8009F8F4;
extern s8 *D_8009F468;
extern s32 D_8009F8B8;
extern volatile s32 D_8009F8E0;
extern s32 D_8009F8E0_arr[];
extern volatile s32 D_8009F8E4;
extern u8 D_8009BB34[];
extern s16 D_8009BB66[];
extern s16 D_8009BBBA[];
extern struct Unk78210Panel D_800A3868;
extern struct Unk78210Panel D_800A3898;
extern struct Unk78210Panel D_800A38C8;

extern void func_80078650(struct Unk78210Panel *panel, s32 arg1);

void func_8007A5F0(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4)
{
    register struct Unk78210Mode *mode __asm__("$6");
    register s8 *p __asm__("$3");

    if (a0 != 0) {
        if (a0 != 3) {
            return;
        }
        mode = D_8009FA94;
        p = D_8009F468;
        mode->unk08 = D_8009F8F0;
        __asm__ volatile("" ::: "memory");
        mode->unk98 = D_8009F8F4;
        if (p[5] == 2) {
            register s32 idx __asm__("$2");
            register u8 *base __asm__("$2");
            idx = D_8009F8B8;
            __asm__ volatile("" ::: "memory");
            idx[D_8009F8E0_arr] = 1;
            __asm__ volatile("" ::: "memory");
            if (D_8009F8E0 == 0 || D_8009F8E4 == 0) {
                return;
            }
            base = D_8009BB34;
            D_8009F8E0 = 0;
            D_8009F8E4 = 0;
            *(s16 *)(base + 0x32) = 0x64;
            *(s16 *)(base + 0x86) = 0x64;
            if (mode->split == 0) {
                mode->unk98 = D_8009F8F0;
                func_80078650(&D_800A3868, 1);
            } else {
                func_80078650(&D_800A3898, 1);
            }
        } else {
            if (mode->split == 0) {
                func_80078650(&D_800A3868, 1);
            } else {
                func_80078650(&D_800A38C8, 1);
            }
        }
    } else {
        register s8 *tbl __asm__("$2");
        register s32 player __asm__("$4");
        tbl = D_8009F468;
        player = D_8009F8B8;
        if (tbl[player] != a4) {
            register s32 off __asm__("$3") = player << 2;
            *(s32 *)((u8 *)D_8009F8E0_arr + off) = 0;
            if (player == 1) {
                D_8009BBBA[0] = 0x64;
            } else {
                D_8009BB66[0] = 0x64;
            }
            player = D_8009F8B8;
        }
        if (D_8009F8E0_arr[player] != 0) {
            if (player == 1) {
                D_8009BBBA[0] = 0x6E;
            } else {
                D_8009BB66[0] = 0x6E;
            }
        }
    }
}
