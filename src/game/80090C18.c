// Categories: Platform > CD-ROM | Resources > File Access
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

extern struct State gSectorBufferPtr __asm__("D_8009CBD0");
extern s32 D_8009CBDC;
extern s32 gCardRetryCount __asm__("D_8009C814");

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

/**
 * @brief Configures and starts (or retries) an asynchronous CD-ROM sector read via D_8009CBD0.
 *
 * Clears active driver callbacks, checks for an open CD shell, optionally pauses and re-seeks on retry, ensures the CD mode byte matches D_8009CBDC, installs sector callback func_800908D4 (and DMA callback func_80090B48 if D_8009C814 bit 0 is set), and issues CdlReadN (command 6).
 *
 * @param arg0 Non-zero indicates a retry (logs retry message, pauses/seeks, and forces CdlSetmode).
 * @return Initial remaining sector count D_8009CBD0.unk14, or -1 on shell-open or command failure.
 *
 * @par Globals
 * D_8009CBD0 (D_8009CBDC at +0xC) - CD read state block; D_8009C814 - bit 0 enables DMA callback; D_800127B4/D_800127CC - shell-open and retry log strings.
 *
 * @par Control flow
 * Clears callbacks; returns -1 after Nop if status bit 0x10 (shell open) is set; on arg0 != 0 logs retry and sends cmd 9 and cmd 2 (Setloc); updates mode with cmd 0xE if needed; installs callbacks and starts cmd 6.
 *
 * @par Calls
 * func_8008E194/func_8008E1A8/func_8008E5D8 - install/clear callbacks; func_8008E010/func_8008E020/func_8008E040/func_8008E0B8 - CD state/loc/sync; func_8008E1BC/func_8008E2F8 - CD commands; func_8008E720 - LBA conversion; func_8008FF08 - BIOS print; func_80091270 - frame timer.
 *
 * @par Side effects
 * Modifies D_8009CBD0 fields, replaces CD callbacks, issues CD-ROM commands, and prints console strings.
 *
 * @par Callers
 * Confirmed: func_800908D4 (0x80090A60), func_80090FA0 (0x80091110), func_8009113C (0x800911BC).
 */
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
    if (gCardRetryCount & 1) {
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
        if (gCardRetryCount & 1) {
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
