#include "game/types.h"

struct Vec3 { s32 x; s32 y; s32 z; };

struct InputAction {
    u32 buttons;
    s32 phase;
};

struct Player {
    u8 pad00[0x30];
    u32 unk30;
    u8 pad34[0x2D0];
    struct Vec3 saved_position;
    s8 unk310;
    u8 pad311[1];
    s8 unk312;
    u8 pad313[1];
    s16 unk314;
    s16 unk316;
    u8 pad318[4];
    struct InputAction unk31C;
};

struct Game {
    u8 pad00[0x24];
    u32 unk24;
    u8 pad28[0x28];
    struct Vec3 position;
    u8 pad5C[0x50];
    struct Player *unkAC;
};

struct Unk9A0 {
    u8 pad00[0x11F];
    u8 unk11F;
};

extern struct Game *D_8009FA58;
extern s32 D_8009F678;
extern u32 D_8009FA00;
extern u32 D_8009FA50;
extern s32 D_8009FA84;
extern u8 *D_8009F2D0;
extern u32 D_8009FA2C;
extern u8 D_8009F680;
extern s32 D_8009FA38;
extern s32 D_8009FA60;
extern u32 D_8009FAA4;
extern s32 D_8009FA30;
extern s32 D_8009FA24;
extern s32 D_8009F9FC;
extern u8 *D_8009F9A0[4];
extern u8 D_8009F2F4[];

extern void func_8005780C(u32 buttons, struct InputAction *action);
extern s32 func_80046C84(s32 id, s32 arg1);
extern void func_80046A34(s32 handle);
extern s32 func_80049340(u8 *text);
extern void func_80049100(u8 *text, s32 x, s32 y, s32 color);
extern void func_80047690(u8 enable);
extern void func_80069D0C(s32 amount);
extern void func_8004CD6C(s32 message, s32 arg2, s32 arg3);
extern void func_8004CE04(void);
extern void func_8001BBAC(void);
extern void func_8001EB6C(void);
extern void func_800476D0(void);
extern void func_8004D598(void);
extern s32 func_8001EF34(s32 arg0);
extern void func_80013C80(void);
extern void func_80026D1C(void);
extern void func_80026D5C(void);
extern void func_80026D9C(s32 arg0);
extern void func_80026DDC(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
extern void func_80026E1C(s32 arg0, s32 arg1);
extern void func_80026E5C(s32 arg0, s32 arg1);
extern void func_80026F68(s32 arg0, s32 arg1);
extern void func_80031DE8(s32 arg0);
extern void func_8003F570(s32 arg0);
extern void func_80047F30(s32 arg0);
extern void func_8004CECC(void);
extern void func_8004DF28(void);
extern void func_800616A0(s32 arg0);

void func_800271A4(void)
{
    struct Player *player;
    struct Unk9A0 *unk;
    u32 flags;
    u8 *text;
    s32 x;
    s32 color;
    s32 clock_value;

    player = D_8009FA58->unkAC;
    func_8005780C(player->unk30, &player->unk31C);

    flags = 0;
    if (player->unk31C.phase == 0 && D_8009FA00 == 4 && player->unk312 == -1) {
        func_80046C84(0x3E, (s32)D_8009FA58);
        *(volatile s16 *)&player->unk316 = 0;
        *(volatile s16 *)&player->unk314 = 0;
        player->saved_position = D_8009FA58->position;
        player->unk310 = !player->unk310;
        if (!player->unk310) {
            if (D_8009F678 != 0) {
                func_80046A34(D_8009F678);
                D_8009F678 = 0;
            }
        }
    }

    if (!(D_8009FA58->unk24 & 0x1000000) && player->unk310 != 0) {
        flags |= 0x21;
    }

    if (D_8009FA50 >= 2 && func_8001EF34(0) == 0 && D_8009FA84 == 0) {
        text = D_8009F2D0;
        x = (0x280 - func_80049340(text)) / 4;
        color = 1;
        if (D_8009FA2C & 0x80) {
            color = 9;
        }
        func_80049100(text, x, 0xB4, color);
        flags |= 0x20;
    }

    if (!(flags & 0x20)) {
        if (D_8009F680 != 0) {
            D_8009FA60 = D_8009FA38;
            clock_value = *(volatile s32 *)&D_8009FA60;
            D_8009FAA4 = clock_value;
            D_8009FA30 = clock_value;
            func_80047690(1);
        }
        D_8009F680 = 0;
        func_80069D0C(D_8009F9FC);
        func_8004DF28();
        func_8004CECC();
        func_8004CD6C(6, D_8009F9FC, 0);
        func_80031DE8(D_8009F9FC);
        func_8004CE04();
        func_8003F570(D_8009F9FC);
    }

    if (flags & 1) {
        func_80026DDC((s32)D_8009FA58, 0, flags & 0x20, 0);
    } else {
        func_80013C80();
        func_80026E5C(0, (s32)D_8009FA58);
        if (!(flags & 0x20)) {
            func_80026D5C();
        }
        func_80026E1C(0, (s32)D_8009FA58);
        func_80026F68(0, 1);
        unk = (struct Unk9A0 *)D_8009F9A0[0];
        if (unk == 0 || unk->unk11F == 0) {
            func_80047F30((s32)D_8009FA58);
        }
    }

    if (D_8009FA84 > 0 && (D_8009FAA4 & 0x100)) {
        func_80049100(D_8009F2F4, 0x91, 0x14, 1);
    }

    func_8001BBAC();

    if (!(flags & 0x20)) {
        func_80026D1C();
        func_800616A0(D_8009F9FC);
        func_80026D9C(D_8009F9FC);
        func_8004D598();
        D_8009FA24 = 0;
    } else {
        if (D_8009F680 == 0) {
            D_8009FA38 = D_8009FA60;
            func_8001EB6C();
            func_800476D0();
            func_80046C84(0x3E, (s32)D_8009FA58);
        }
        D_8009F680 = 1;
        D_8009FA24 = 1;
    }
}
