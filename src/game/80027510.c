#include "game/types.h"

struct Vec3 { s32 x, y, z; };
struct InputAction { u32 buttons; s32 phase; };
struct Player {
    u8 pad00[0x30];
    u32 unk30;
    u8 pad34[0x78];
    s8 unkAC;
    u8 padAD[0x257];
    struct Vec3 saved_position;
    s8 unk310;
    u8 pad311;
    s8 unk312;
    u8 pad313;
    s16 unk314;
    s16 unk316;
    u8 pad318[4];
    struct InputAction unk31C;
    s16 unk324;
};
struct Game {
    u8 pad00[0x24];
    u32 unk24;
    u8 pad28[0x28];
    struct Vec3 position;
    u8 pad5C[0x50];
    struct Player *unkAC;
    u8 padB0[0x2C];
    struct Game *unkDC;
};
struct Menu {
    u8 pad00[0xC];
    s32 unkC;
    u8 pad10[0x4C];
    s32 unk5C;
};
extern union { struct Game *ptr; u32 raw; } D_8009FA58;
extern struct Game *D_8009FA58_arr[];
extern struct { struct Game *ptr; u32 pad[2]; } D_8009FA5C;
extern s32 D_8009F678[];
extern struct { struct Game *ptr; u32 pad[2]; } D_8009FA5C_b, D_8009FA5C_c, D_8009FA5C_d, D_8009FA5C_e;
extern s32 D_8009FA00;
extern struct Menu *D_8009FA94;
extern s32 D_8009FA6C;
extern u32 D_8009FA50;
extern s32 D_8009FA84;
extern u8 D_8009F680;
extern s32 D_8009FA38;
extern s32 D_8009FA60;
extern u32 D_8009FAA4;
extern s32 D_8009FA30;
extern s32 D_8009F9FC;
extern u8 *D_8009F2D0;
extern u8 D_8009F2F4[];
extern u32 D_8009FA2C;
extern s32 D_8009FA24;

extern void func_8005780C(u32, struct InputAction *);
extern void func_80046A34(s32);
extern s32 func_80046C84(s32, struct Game *);
extern s32 func_8001EF34(s32);
extern void func_80047690(s32);
extern void func_80069D0C(s32);
extern void func_8004E248(void);
extern void func_8004CECC(void);
extern void func_8004CD6C(s32, s32, s32);
extern void func_80031DE8(s32);
extern void func_8004CE04(void);
extern void func_8003F570(s32);
extern void func_80026E5C(s32, struct Game *);
extern void func_80026D5C(void);
extern void func_80017B60(s32);
extern void func_80013C80(void);
extern void func_80026DDC(struct Game *, s32, s32, s32);
extern void func_8001C214(u32, s32, s32, s32, s32, u8);
extern void func_80026E1C(s32, struct Game *);
extern void func_80047F30(struct Game *);
extern void func_80026F68(s32, s32);
extern s32 func_80049340(u8 *);
extern void func_80049100(u8 *, s32, s32, s32);
extern void func_800485AC(s32);
extern void func_800486FC(s32);
extern void func_8001BBAC(void);
extern void func_80026D1C(void);
extern void func_800616A0(s32);
extern void func_80026D9C(s32);
extern void func_8004D598(void);
extern void func_8001EB6C(void);
extern void func_800476D0(void);

void func_80027510(void)
{
    u32 flags;
    s32 i;
    struct Game *game;
    struct Game *game2;
    s32 old_flags;
    s32 old_flags2;
    s32 clock_value;
    u8 *text;
    register s32 x __asm__("$3");
    register s32 xarg __asm__("$5");
    s32 color;
    register s32 index __asm__("$2");
    register struct Game **base __asm__("$2");
    register struct Game **endbase __asm__("$3");
    register s32 zero_arg __asm__("$4");
    register u8 *banner __asm__("$4");

    flags = 0;
    __asm__ volatile("" : "=r"(i) : "0"(flags));
    for (;;) {
        base = D_8009FA58_arr;
        __asm__ volatile("" : "=r"(base) : "0"(base));
        game = base[i];
        func_8005780C(game->unkAC->unk30, &game->unkAC->unk31C);
        if (game->unkAC->unk31C.phase == 0 && D_8009FA00 == 4 && game->unkAC->unk312 == -1) {
            game->unkAC->unk316 = 0;
            game->unkAC->unk314 = 0;
            game->unkAC->saved_position = game->position;
            game->unkAC->unk310 = !game->unkAC->unk310;
            if (!game->unkAC->unk310) {
                if (D_8009F678[i] != 0) {
                    func_80046A34(D_8009F678[i]);
                    D_8009F678[i] = 0;
                }
                func_80046C84(0x3E, game);
            } else if (D_8009FA94->unk5C == 1 && D_8009FA94->unkC == 2) {
                func_80046C84(0x3E, game);
            }
        }
        i++;
        if (i >= 2) break;
    }
    if (!(D_8009FA58.ptr->unk24 & 0x1000000) && D_8009FA58.ptr->unkAC->unk310 != 0) {
        flags |= 1;
        if (D_8009FA94->unk5C == 0 || D_8009FA6C != 0) flags |= 0x20;
    }
    if (!(D_8009FA5C.ptr->unk24 & 0x1000000) && D_8009FA5C.ptr->unkAC->unk310 != 0) {
        flags |= 2;
        if (D_8009FA94->unk5C == 0 || D_8009FA6C != 0) flags |= 0x20;
    }
    if (D_8009FA50 >= 2 && (func_8001EF34(0) == 0 || func_8001EF34(1) == 0) && D_8009FA84 == 0) {
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
        func_8004E248();
        func_8004CECC();
        func_8004CD6C(6, D_8009F9FC, 0);
        func_80031DE8(D_8009F9FC);
        func_8004CE04();
        func_8003F570(D_8009F9FC);
    }
    if (D_8009FA58.ptr != 0 && !(flags & 1)) func_80026E5C(0, D_8009FA58.ptr);
    if (D_8009FA5C.ptr != 0 && !(flags & 2)) func_80026E5C(1, D_8009FA5C.ptr);
    if (!(flags & 0x20)) func_80026D5C();
    if (D_8009FA58.ptr != 0) {
        old_flags = 1;
        game2 = D_8009FA5C.ptr;
        if (game2 != 0 && game2->unkAC->unk324 > 0) {
            old_flags = game2->unk24 & 0x20;
            game2->unk24 |= 0x20;
            D_8009FA5C.ptr->unkDC->unk24 |= 0x20;
        }
        func_80017B60(D_8009FA58.ptr->unkAC->unkAC);
        func_80013C80();
        if (flags & 1) {
            func_80026DDC(D_8009FA58.ptr, 0, flags & 0x20, D_8009FA94->unk5C == 1);
            func_8001C214(1, 0x13E, 0, 2, 0x1E0, 0);
        } else {
            func_80026E1C(0, D_8009FA58.ptr);
            func_80047F30(D_8009FA58.ptr);
        }
        zero_arg = 0;
        __asm__ volatile("" : "=r"(zero_arg) : "0"(zero_arg));
        func_80026F68(zero_arg, zero_arg);
        if (D_8009FA84 > 0 && (D_8009FAA4 & 0x100)) {
            banner = (u8 *)0x800A0000;
            __asm__ volatile("" : "=r"(banner) : "0"(banner));
            func_80049100(banner - 0xD0C, 0x41, 0x14, 1);
        }
        if (!old_flags) {
            D_8009FA5C.ptr->unk24 &= ~0x20;
            D_8009FA5C.ptr->unkDC->unk24 &= ~0x20;
        }
        if (D_8009FA50 >= 2 && func_8001EF34(0) == 0 && D_8009FA84 == 0) {
            text = D_8009F2D0;
            x = 0x140 - func_80049340(text);
            color = 1;
            if (x < 0) x += 3;
            xarg = x >> 2;
            if (D_8009FA2C & 0x80) color = 9;
            func_80049100(text, xarg, 0xB4, color);
        }
        func_800485AC(0);
        func_8001BBAC();
        old_flags2 = 1;
        if (D_8009FA5C.ptr != 0) {
            if (D_8009FA58.ptr->unkAC->unk324 > 0) {
                old_flags2 = D_8009FA58.ptr->unk24 & 0x20;
                D_8009FA58.ptr->unk24 |= 0x20;
                D_8009FA58.ptr->unkDC->unk24 |= 0x20;
            }
            func_80017B60(D_8009FA5C_b.ptr->unkAC->unkAC);
            func_80013C80();
            if (flags & 2) {
                func_80026DDC(D_8009FA5C_c.ptr, 1, flags & 0x20, D_8009FA94->unk5C == 1);
            } else {
                func_80026E1C(1, D_8009FA5C_d.ptr);
                func_80047F30(D_8009FA5C_e.ptr);
            }
        }
        func_80026F68(1, 0);
        if (D_8009FA84 > 0 && (D_8009FAA4 & 0x100)) {
            banner = (u8 *)0x800A0000;
            __asm__ volatile("" : "=r"(banner) : "0"(banner));
            func_80049100(banner - 0xD0C, 0x41, 0x14, 1);
        }
        if (D_8009FA50 >= 2 && func_8001EF34(1) == 0 && D_8009FA84 == 0) {
            text = D_8009F2D0;
            x = 0x140 - func_80049340(text);
            color = 1;
            if (x < 0) x += 3;
            xarg = x >> 2;
            if (D_8009FA2C & 0x80) color = 9;
            func_80049100(text, xarg, 0xB4, color);
        }
        func_800485AC(1);
        func_800486FC(1);
        if (!old_flags2) {
            D_8009FA58.ptr->unk24 &= ~0x20;
            D_8009FA58.ptr->unkDC->unk24 &= ~0x20;
        }
        func_8001BBAC();
    }
    func_80017B60(D_8009FA58.ptr->unkAC->unkAC);
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
            if (D_8009FA94->unk5C == 0) {
                index = flags & 1;
                __asm__ volatile("" : "=r"(index) : "0"(index));
                index = !index;
            } else index = D_8009FA58.ptr->unkAC->unk312 != 0;
            endbase = D_8009FA58_arr;
            __asm__ volatile("" : "=r"(endbase) : "0"(endbase));
            func_80046C84(0x3E, endbase[index]);
        }
        D_8009F680 = 1;
        D_8009FA24 = 1;
    }
}
