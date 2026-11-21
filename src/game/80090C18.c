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

extern struct State gCdAsyncReadState __asm__("D_8009CBD0");
extern s32 gCdReadModeWord __asm__("D_8009CBDC");
extern s32 gCdUseDmaFlag __asm__("D_8009C814");

extern s32 gSetCdSyncCallback(s32) __asm__("func_8008E194");
extern s32 gSetCdReadyCallback(void *) __asm__("func_8008E1A8");
extern void gSetCdDmaCallback(void *) __asm__("func_8008E5D8");
extern s32 gGetCdStatusByte(void) __asm__("func_8008E010");
extern s32 gVSyncWait(s32) __asm__("func_80091270");
extern void gBiosPuts(char *) __asm__("func_8008FF08");
extern s32 gCdControlF(s32, s32) __asm__("func_8008E2F8");
extern s32 gCdControl(s32, void *, s32) __asm__("func_8008E1BC");
extern void *gGetCdTargetLocation(void) __asm__("func_8008E040");
extern void gResetCdController(void) __asm__("func_8008E0B8");
extern s32 gGetCdModeByte(void) __asm__("func_8008E020");
extern s32 gCdPosToInt(void *) __asm__("func_8008E720");

extern void gCdDataReadyCallback(void) __asm__("func_800908D4");
extern void gCdDmaCompleteCb(void) __asm__("func_80090B48");
s32 gStartCdSectorRead(s32 arg0) __asm__("func_80090C18");


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
 *
 * @par Role and path
 * Starts or retries an asynchronous CD-ROM sector read using D_8009CBD0,
 * clearing active callbacks and returning -1 if shell-open status bit 0x10 is
 * set in func_8008E010. On retry (arg0 != 0) it logs, pauses, and re-seeks
 * via func_8008E1BC; it updates the CD mode to D_8009CBDC if needed, installs
 * func_800908D4 via func_8008E1A8 and func_80090B48 via func_8008E5D8 when
 * D_8009C814 & 1, and starts CdlReadN (6) via func_8008E2F8. Driven by
 * func_80090FA0, func_8009113C, and func_800908D4.
 */
s32 gStartCdSectorRead(s32 arg0)
{
    register struct State *state __asm__("$16");
    register s32 *mode_ptr __asm__("$17");
    register s32 arg __asm__("$18") = arg0;
    register s32 mode __asm__("$16");
    volatile u8 mode_val;
    s32 pos;

    gSetCdSyncCallback(0);
    gSetCdReadyCallback(0);
    if (gCdUseDmaFlag & 1) {
        gSetCdDmaCallback(0);
    }
    if (gGetCdStatusByte() & 0x10) {
        u32 t;
        register struct State *s __asm__("$3");
        if ((gVSyncWait(-1) & 0x3F) == 0) {
            register char *base __asm__("$4") = (char *)0x80010000;
            __asm__("" : "=r"(base) : "0"(base));
            gBiosPuts(base + 0x27B4);
        }
        gCdControlF(1, 0);
        t = gVSyncWait(-1);
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
            gBiosPuts(base + 0x27CC);
            gCdControl(9, 0, 0);
            if (gCdControl(2, gGetCdTargetLocation(), 0) == 0) {
                goto fail;
            }
        }
    }

    gResetCdController();
    __asm__ volatile("");
    mode_ptr = (s32 *)0x800A0000;
    __asm__("" : "=r"(mode_ptr) : "0"(mode_ptr));
    mode_ptr = (s32 *)((char *)mode_ptr - 0x3424);
    __asm__("" : "=r"(mode_ptr) : "0"(mode_ptr));
    mode = *mode_ptr;
    mode_val = mode;
    mode &= 0xFF;
    if (mode != gGetCdModeByte() || arg != 0) {
        if (gCdControl(0xE, (void *)&mode_val, 0) == 0) {
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

    pos = gCdPosToInt(gGetCdTargetLocation());
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
        gSetCdReadyCallback(base);
    }
    {
        register s32 six __asm__("$4") = 6;
        if (gCdUseDmaFlag & 1) {
            register char *base __asm__("$4") = (char *)0x80090000;
            __asm__("" : "=r"(base) : "0"(base));
            gSetCdDmaCallback(base + 0x0B48);
            six = 6;
        }
        state->unk8 = (s32)state->buffer;
        gCdControlF(six, 0);
    }
    state->unk14 = state->count;
    state->unk18 = gVSyncWait(-1);
    __asm__ volatile(".set reorder");
    return *(volatile s32 *)&state->unk14;
}
