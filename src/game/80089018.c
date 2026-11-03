#include "game/types.h"

struct Sub80089018 {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    u8 pad0C[0x24];
    s32 unk30;
    s32 unk34;
};

struct State80089018 {
    volatile u32 unk000;
    u32 unk004;
    u32 unk008;
    u8 pad00C[0x3FC];
    volatile u32 unk408;
    volatile u32 unk40C;
    volatile u32 unk410;
    volatile u32 unk414;
    volatile u32 unk418;
    volatile u32 unk41C;
    u8 pad420[4];
    volatile u32 unk424;
    u8 pad428[8];
    struct Sub80089018 *unk430;
    u8 pad434[8];
    u32 unk43C;
    volatile u32 unk440;
    volatile u32 unk444;
    volatile u32 unk448;
    volatile u32 unk44C;
    u8 pad450[0x120];
    void (*unk570)(void);
};

extern struct State80089018 *D_8009F970;
extern u32 D_8009F980;
extern u8 D_80088D74[8];

extern void func_8008C9C0(void);
extern void func_8008C9E0(void);
extern s32 func_8008E194(s32 value);
extern s32 func_8008E1A8(s32 value);
extern void func_8008E1BC(s32 arg0, s32 arg1, s32 arg2);
extern s32 func_800886E4(void);
extern void func_8008E42C(s32 arg0, void *arg1, s32 arg2);
extern void func_800889DC(s32 arg0, s32 arg1, s32 arg2);
extern s32 func_80088770(s32 arg0, s32 arg1, s32 arg2);
extern void func_80088974(s32 arg0, s32 arg1, s32 arg2);
extern void func_80088F00(void);
extern s32 func_800886C8(s32 a, s32 b);

void func_80089018(void)
{
    u8 local;
    s32 work;

    if (D_8009F970->unk424 >= 2) {
        if (D_8009F970->unk570 != 0) {
            D_8009F970->unk570();
        }
        func_8008C9C0();
        D_8009F970->unk410 = 0;
        D_8009F970->unk408 = 0;
        D_8009F970->unk40C = 0;
        D_8009F970->unk41C = 0;
        func_8008E194(0);
        func_8008E1A8(0);
        func_8008C9E0();
        func_8008E1BC(8, 0, 0);
        while (func_800886E4() != 0) {
        }
        local = 0xA0;
        func_8008E42C(0xE, &local, 0);
        work = 1;
        func_8008C9C0();
        func_8008E194((s32)D_80088D74);
        func_8008E1A8((s32)func_800889DC + 0x40);
        D_8009F970->unk41C = work;
        D_8009F970->unk414 = work;
        D_8009F970->unk424 = 0;
        func_8008C9E0();
        if ((D_8009F970->unk414 != 0) && (D_8009F970->unk000 != 0)) {
            {
                register struct Sub80089018 *parameter __asm__("$5");
                register s32 address __asm__("$4");
                register s32 sector __asm__("$6");

                parameter = D_8009F970->unk430;
                address = (s32)((u8 *)parameter + 0xC);
                sector = parameter->unk08;
                parameter = (struct Sub80089018 *)((u8 *)parameter + 0x24);
                func_800889DC(address, (s32)parameter, sector);
            }
            {
                register struct State80089018 *state __asm__("$3");
                register struct Sub80089018 *parameter __asm__("$5");

                state = D_8009F970;
                parameter = state->unk430;
                state->unk418 = 2;
                parameter = (struct Sub80089018 *)((u8 *)parameter + 0x24);
                if (func_80088770(0x15, (s32)parameter, 0) == 0) {
                    D_8009F970->unk43C = work;
                    return;
                }
            }
            work = (s32)D_8009F970->unk000;
            func_80088974(D_8009F970->unk004, work,
                          (s32)((u8 *)D_8009F970->unk430 + 0x24));
        } else {
            D_8009F970->unk414 = 0;
        }
    }

    if (D_8009F970->unk414 != 0) {
        return;
    }

    {
        struct Sub80089018 *sub;

        sub = D_8009F970->unk430;
        work = sub->unk00 - sub->unk08;
    }
    if (work == 0) {
        func_80088F00();
    }

    {
        register struct State80089018 *state __asm__("$2");
        register u32 read_position __asm__("$4");
        register u32 capacity __asm__("$5");
        register u32 write_position __asm__("$6");
        register u32 dividend __asm__("$2");
        register u32 remainder __asm__("$3");

        state = D_8009F970;
        capacity = D_8009F980;
        read_position = state->unk440;
        write_position = state->unk448;
        dividend = state->unk448;
        remainder = dividend % capacity;
        read_position += capacity;
        read_position -= write_position;
        capacity -= remainder;
        work = func_800886C8(func_800886C8((s32)read_position, (s32)capacity), work);
    }

    if (work != 0) {
        {
            register struct Sub80089018 *parameter __asm__("$5");
            register s32 address __asm__("$4");
            register s32 sector __asm__("$6");

            parameter = D_8009F970->unk430;
            address = (s32)((u8 *)parameter + 0xC);
            sector = parameter->unk08;
            parameter = (struct Sub80089018 *)((u8 *)parameter + 0x24);
            func_800889DC(address, (s32)parameter, sector);
        }
        {
            register struct State80089018 *state __asm__("$3");
            register struct Sub80089018 *parameter __asm__("$5");

            state = D_8009F970;
            parameter = state->unk430;
            parameter = (struct Sub80089018 *)((u8 *)parameter + 0x24);
            state->unk44C = state->unk448 + work;
            state->unk418 = 2;
            if (func_80088770(0x15, (s32)parameter, 0) == 0) {
                D_8009F970->unk43C = 1;
                return;
            }
        }
        {
            register struct State80089018 *state __asm__("$7");
            register struct Sub80089018 *sub __asm__("$8");
            register u32 dividend __asm__("$2");
            register u32 address __asm__("$4");
            register u32 divisor __asm__("$3");
            register u8 *parameter __asm__("$6");
            register u32 start __asm__("$3");

            state = D_8009F970;
            sub = state->unk430;
            dividend = state->unk448;
            address = sub->unk34;
            divisor = D_8009F980;
            dividend += address;
            address = dividend % divisor;
            parameter = (u8 *)sub;
            __asm__("" : "+r"(parameter));
            parameter += 0x24;
            start = sub->unk30;
            state->unk414 = 1;
            address <<= 11;
            func_80088974((s32)(start + address), work, (s32)parameter);
        }
    }
}
