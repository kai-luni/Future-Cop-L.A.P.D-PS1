// Categories: Platform > Buffered Transfers
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

extern struct State80089018 *gMovieStreamContext __asm__("D_8009F970");
extern u32 gMovieRingSectors __asm__("D_8009F980");
extern u8 gCodeDataSource[8] __asm__("D_80088D74");

extern void gDisableCpuInterrupts(void) __asm__("func_8008C9C0");
extern void gEnableCpuInterrupts(void) __asm__("func_8008C9E0");
extern s32 gSetCdSyncCallback(s32 value) __asm__("func_8008E194");
extern s32 gSetCdReadyCallback(s32 value) __asm__("func_8008E1A8");
extern void gCdControl(s32 arg0, s32 arg1, s32 arg2) __asm__("func_8008E1BC");
extern s32 func_800886E4(void);
extern void gCdControlB(s32 arg0, void *arg1, s32 arg2) __asm__("func_8008E42C");
extern void func_800889DC(s32 arg0, s32 arg1, s32 arg2);
extern s32 func_80088770(s32 arg0, s32 arg1, s32 arg2);
extern void func_80088974(s32 arg0, s32 arg1, s32 arg2);
extern void gMovieStreamPrimeEntry(void) __asm__("func_80088F00");
extern s32 gMinS32(s32 a, s32 b) __asm__("func_800886C8");

/**
 *  @brief Resets transfer state when requested and starts a bounded buffered transfer.
 *
 *  Uses a shared state structure and ring capacity to prepare a command and
 *  schedule either a special transfer or a contiguous portion of remaining work.
 *  @par Globals
 *  gMovieStreamContext holds transfer state, parameters, and an optional callback;
 *  D_8009F980 supplies ring capacity and D_80088D74 supplies a callback address.
 *  @par Control flow
 *  A reset branch invokes the callback, waits for readiness, and reinitializes state.
 *  Active transfers return early; otherwise the work size is bounded by ring space
 *  and the contiguous span. A failed command sets the state error flag and returns.
 *  @par Calls
 *  func_8008C9C0 and func_8008C9E0 bracket state changes; func_8008E194,
 *  func_8008E1A8, func_8008E1BC, and func_8008E42C configure the operation.
 *  func_800886E4 polls readiness; func_800889DC prepares parameters;
 *  func_80088770 issues a command and func_80088974 starts a transfer.
 *  gMovieStreamPrimeEntry handles zero remaining work; func_800886C8 bounds the size.
 *  @par Side effects
 *  Updates shared transfer fields, invokes an optional callback, and may start I/O.
 *  @par Callers
 *  Confirmed: gMovieStreamAdvance.
 */
void func_80089018(void)
{
    u8 local;
    s32 work;

    if (gMovieStreamContext->unk424 >= 2) {
        if (gMovieStreamContext->unk570 != 0) {
            gMovieStreamContext->unk570();
        }
        gDisableCpuInterrupts();
        gMovieStreamContext->unk410 = 0;
        gMovieStreamContext->unk408 = 0;
        gMovieStreamContext->unk40C = 0;
        gMovieStreamContext->unk41C = 0;
        gSetCdSyncCallback(0);
        gSetCdReadyCallback(0);
        gEnableCpuInterrupts();
        gCdControl(8, 0, 0);
        while (func_800886E4() != 0) {
        }
        local = 0xA0;
        gCdControlB(0xE, &local, 0);
        work = 1;
        gDisableCpuInterrupts();
        gSetCdSyncCallback((s32)gCodeDataSource);
        gSetCdReadyCallback((s32)func_800889DC + 0x40);
        gMovieStreamContext->unk41C = work;
        gMovieStreamContext->unk414 = work;
        gMovieStreamContext->unk424 = 0;
        gEnableCpuInterrupts();
        if ((gMovieStreamContext->unk414 != 0) && (gMovieStreamContext->unk000 != 0)) {
            {
                register struct Sub80089018 *parameter __asm__("$5");
                register s32 address __asm__("$4");
                register s32 sector __asm__("$6");

                parameter = gMovieStreamContext->unk430;
                address = (s32)((u8 *)parameter + 0xC);
                sector = parameter->unk08;
                parameter = (struct Sub80089018 *)((u8 *)parameter + 0x24);
                func_800889DC(address, (s32)parameter, sector);
            }
            {
                register struct State80089018 *state __asm__("$3");
                register struct Sub80089018 *parameter __asm__("$5");

                state = gMovieStreamContext;
                parameter = state->unk430;
                state->unk418 = 2;
                parameter = (struct Sub80089018 *)((u8 *)parameter + 0x24);
                if (func_80088770(0x15, (s32)parameter, 0) == 0) {
                    gMovieStreamContext->unk43C = work;
                    return;
                }
            }
            work = (s32)gMovieStreamContext->unk000;
            func_80088974(gMovieStreamContext->unk004, work,
                          (s32)((u8 *)gMovieStreamContext->unk430 + 0x24));
        } else {
            gMovieStreamContext->unk414 = 0;
        }
    }

    if (gMovieStreamContext->unk414 != 0) {
        return;
    }

    {
        struct Sub80089018 *sub;

        sub = gMovieStreamContext->unk430;
        work = sub->unk00 - sub->unk08;
    }
    if (work == 0) {
        gMovieStreamPrimeEntry();
    }

    {
        register struct State80089018 *state __asm__("$2");
        register u32 read_position __asm__("$4");
        register u32 capacity __asm__("$5");
        register u32 write_position __asm__("$6");
        register u32 dividend __asm__("$2");
        register u32 remainder __asm__("$3");

        state = gMovieStreamContext;
        capacity = gMovieRingSectors;
        read_position = state->unk440;
        write_position = state->unk448;
        dividend = state->unk448;
        remainder = dividend % capacity;
        read_position += capacity;
        read_position -= write_position;
        capacity -= remainder;
        work = gMinS32(gMinS32((s32)read_position, (s32)capacity), work);
    }

    if (work != 0) {
        {
            register struct Sub80089018 *parameter __asm__("$5");
            register s32 address __asm__("$4");
            register s32 sector __asm__("$6");

            parameter = gMovieStreamContext->unk430;
            address = (s32)((u8 *)parameter + 0xC);
            sector = parameter->unk08;
            parameter = (struct Sub80089018 *)((u8 *)parameter + 0x24);
            func_800889DC(address, (s32)parameter, sector);
        }
        {
            register struct State80089018 *state __asm__("$3");
            register struct Sub80089018 *parameter __asm__("$5");

            state = gMovieStreamContext;
            parameter = state->unk430;
            parameter = (struct Sub80089018 *)((u8 *)parameter + 0x24);
            state->unk44C = state->unk448 + work;
            state->unk418 = 2;
            if (func_80088770(0x15, (s32)parameter, 0) == 0) {
                gMovieStreamContext->unk43C = 1;
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

            state = gMovieStreamContext;
            sub = state->unk430;
            dividend = state->unk448;
            address = sub->unk34;
            divisor = gMovieRingSectors;
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
