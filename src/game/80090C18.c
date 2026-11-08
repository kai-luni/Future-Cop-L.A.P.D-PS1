#include "game/types.h"

struct State {
    s32 count;
    void *buffer;
    s32 unk8;
    s32 mode;
    s32 command;
    s32 unk14;
    s32 unk18;
    u32 time;
    s32 unk20;
    s32 busy;
};

extern struct State D_8009CBD0;
extern s32 D_8009CBDC;
extern s32 D_8009C814;

extern s32 func_8008E194(s32);
extern s32 func_8008E1A8(void *);
extern void func_8008E5D8(void *);
extern s32 func_8008E010(void);
extern s32 func_80091270(s32);
extern void func_8008FF08(char *);
extern s32 func_8008E2F8(s32, s32);
extern s32 func_8008E1BC(s32, void *, s32);
extern void *func_8008E040(void);
extern void func_8008E0B8(void);
extern s32 func_8008E020(void);
extern s32 func_8008E720(void *);

extern void func_800908D4(void);
extern void func_80090B48(void);

s32 func_80090C18(s32 arg0)
{
    register struct State *state __asm__("$16");
    register s32 *mode_ptr __asm__("$17");
    register s32 arg __asm__("$18") = arg0;
    register s32 mode __asm__("$16");
    volatile u8 mode_val;
    s32 pos;

    func_8008E194(0);
    func_8008E1A8(0);
    if (D_8009C814 & 1) {
        func_8008E5D8(0);
    }
    if (func_8008E010() & 0x10) {
        u32 t;
        register struct State *s __asm__("$3");
        if ((func_80091270(-1) & 0x3F) == 0) {
            register char *base __asm__("$4") = (char *)0x80010000;
            __asm__("" : "=r"(base) : "0"(base));
            func_8008FF08(base + 0x27B4);
        }
        func_8008E2F8(1, 0);
        t = func_80091270(-1);
        s = (struct State *)0x800A0000;
        __asm__("" : "=r"(s) : "0"(s));
        s = (struct State *)((char *)s - 0x3430);
        s->time = t;
        s->unk14 = -1;
        return *(volatile s32 *)&s->unk14;
    }

    if (arg != 0) {
        __asm__ volatile("");
        {
            register char *base __asm__("$4") = (char *)0x80010000;
            __asm__("" : "=r"(base) : "0"(base));
            func_8008FF08(base + 0x27CC);
            func_8008E1BC(9, 0, 0);
            if (func_8008E1BC(2, func_8008E040(), 0) == 0) {
                goto fail;
            }
        }
    }

    func_8008E0B8();
    __asm__ volatile("");
    mode_ptr = (s32 *)0x800A0000;
    __asm__("" : "=r"(mode_ptr) : "0"(mode_ptr));
    mode_ptr = (s32 *)((char *)mode_ptr - 0x3424);
    __asm__("" : "=r"(mode_ptr) : "0"(mode_ptr));
    mode = *mode_ptr;
    mode_val = mode;
    mode &= 0xFF;
    if (mode != func_8008E020() || arg != 0) {
        if (func_8008E1BC(0xE, (void *)&mode_val, 0) == 0) {
fail:
            __asm__ volatile("");
            {
                register struct State *s __asm__("$2") = (struct State *)0x800A0000;
                __asm__("" : "=r"(s) : "0"(s));
                s = (struct State *)((char *)s - 0x3430);
                s->unk14 = -1;
                return *(volatile s32 *)&s->unk14;
            }
        }
    }

    pos = func_8008E720(func_8008E040());
    __asm__ volatile("" : : "r"(mode));
    __asm__ volatile("");
    {
        register char *base __asm__("$4") = (char *)0x80090000;
        __asm__("" : "=r"(base) : "0"(base));
        base += 0x08D4;
        state = (struct State *)0x800A0000;
        __asm__("" : "=r"(state) : "0"(state));
        state = (struct State *)((char *)state - 0x3430);
        __asm__("" : "=r"(state) : "0"(state));
        state->unk20 = pos;
        func_8008E1A8(base);
    }
    {
        register s32 six __asm__("$4") = 6;
        if (D_8009C814 & 1) {
            register char *base __asm__("$4") = (char *)0x80090000;
            __asm__("" : "=r"(base) : "0"(base));
            func_8008E5D8(base + 0x0B48);
            six = 6;
        }
        state->unk8 = (s32)state->buffer;
        func_8008E2F8(six, 0);
    }
    state->unk14 = state->count;
    state->unk18 = func_80091270(-1);
    __asm__ volatile(".set reorder");
    return *(volatile s32 *)&state->unk14;
}
