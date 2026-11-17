// Categories: Platform > CD-ROM | Resources > File Access
#include "game/types.h"

struct CdlLOC {
    u8 minute;
    u8 second;
    u8 sector;
    u8 track;
};

struct State {
    s32 count;
    void *buffer;
    volatile s32 unk8;
    s32 mode;
    s32 command;
    volatile s32 unk14;
    volatile s32 unk18;
    s32 time;
    s32 unk20;
    s32 busy;
    s32 callback, callback2, callback3;
    volatile s32 unk34;
};

struct Local {
    struct CdlLOC loc;
    u8 pad[0xC];
};

extern struct State gSectorBufferPtr __asm__("D_8009CBD0");
extern s32 D_8009CBD8[];
extern s32 D_8009CBF0[];
extern s32 D_8009C814;
extern void (*D_8009C810)(s32, s32);
extern s32 D_800908A0[];
extern char D_8001279C[];
extern void func_80090B48(void);

extern s32 func_8008E5D8(s32);
extern s32 func_8008E5B8(void *, s32);
extern s32 func_8008E598(void *, s32);
extern s32 func_8008E5FC(s32);
extern s32 func_8008E720(struct CdlLOC *);
extern void func_8008FF08(char *);
extern s32 func_80091270(s32);
extern s32 func_80090C18(s32);
extern s32 func_8008E1A8(s32);
extern s32 func_8008E194(s32);
extern void func_8008E2F8(s32, s32);

/**
 * @brief CD-ROM data-ready callback that reads sectors, checks LBA alignment, and completes or retries transfers.
 *
 * Installed into D_8009C8AC by func_80090C18. On mode == 1 while unk14 > 0, validates the 3-byte sector header against unk20 for command 0x200 (aborting with D_8001279C on mismatch), reads a sector via DMA or PIO, triggers func_80090C18(1) if unk14 < 0, and cleans up callbacks plus invokes D_8009C810 on completion or 0x4B0-tick timeout.
 *
 * @param mode Event status byte (1 = sector data ready; other values abort with unk14 = -1).
 * @param arg1 Driver callback argument saved in D_8009CBD0.unk34 and passed to D_8009C810.
 * @par Globals
 * D_8009CBD0 (D_8009CBD8 at +8, D_8009CBF0 at +0x20), D_8009C814 (bit 0 selects DMA), D_8009C810 (completion callback), D_8001279C ('CdRead: sector error\n'), func_800908A0/func_80090B48.
 * @par Control flow
 * Branches on mode == 1 and unk14 > 0; checks sector LBA when command == 0x200; transfers sector via DMA or PIO; retries via func_80090C18(1) if unk14 < 0; finishes on unk14 == 0 or timeout.
 * @par Calls
 * func_8008E5D8/func_8008E5B8/func_8008E598/func_8008E5FC (CD sector reads), func_8008E720 (LBA), func_8008FF08 (BIOS print), func_80091270 (timer), func_80090C18 (retry), func_8008E1A8/func_8008E194/func_8008E2F8 (restore/pause).
 * @par Side effects
 * Writes sector data to D_8009CBD0.unk8, updates D_8009CBD0, prints errors, restores CD callbacks, pauses CD, and invokes D_8009C810.
 * @par Callers
 * Confirmed: none direct; func_80090C18 passes address D_800908D4 to func_8008E1A8 at 0x80090D80.
 */
void func_800908D4(u8 mode, s32 arg1)
{
    struct Local tmp;
    register s32 saved __asm__("$17") = arg1;
    register s32 *p __asm__("$16");
    struct State *st;
    s32 ret;

    gSectorBufferPtr.unk34 = saved;
    if (mode == 1) {
        if (gSectorBufferPtr.unk14 > 0) {
            __asm__ volatile("");
            if (gSectorBufferPtr.command == 0x200) {
                if (D_8009C814 & 1) {
                    func_8008E5D8(0);
                    func_8008E5B8(&tmp.loc, 3);
                    func_8008E5FC(0);
                    func_8008E5D8((s32)func_80090B48);
                } else {
                    func_8008E598(&tmp.loc, 3);
                }
                ret = func_8008E720(&tmp.loc);
                p = D_8009CBF0;
                __asm__ volatile("" : "+r"(p));
                if (ret != *p) {
                    register struct State *s __asm__("$3");
                    func_8008FF08(D_8001279C);
                    s = (struct State *)(p - 8);
                    __asm__("" : "+r"(s));
                    s->unk14 = -1;
                }
            }
            if (D_8009C814 & 1) {
                s32 *q;
                __asm__ volatile("" : : : "memory");
                q = D_8009CBD8;
                __asm__("" : "+r"(q));
                func_8008E5B8((void *)q[0], q[2]);
            } else {
                register struct State *s __asm__("$4");
                __asm__ volatile("" : : : "memory");
                p = D_8009CBD8;
                __asm__ volatile("" : "+r"(p));
                func_8008E598((void *)p[0], p[2]);
                s = (struct State *)(p - 2);
                __asm__("" : "+r"(s));
                s->unk8 = p[0] + p[2] * 4;
                s->unk14--;
                __asm__ volatile("");
                s->unk20++;
            }
        }
    } else {
        gSectorBufferPtr.unk14 = -1;
    }

    {
        s32 t = func_80091270(-1);
        st = &gSectorBufferPtr;
        __asm__("" : "+r"(st));
        st->unk18 = t;
    }
    if (st->unk14 < 0) {
        func_80090C18(1);
    }
    if (func_80091270(-1) > st->time + 0x4B0) {
        st->unk14 = -1;
    }
    if (st->unk14 == 0 || func_80091270(-1) > st->time + 0x4B0) {
        func_8008E1A8(st->callback2);
        if (D_8009C814 & 1) {
            func_8008E5D8(st->callback3);
        }
        func_8008E194((s32)D_800908A0);
        func_8008E2F8(9, 0);
        st->busy = 1;
        if (D_8009C810) {
            D_8009C810(st->unk14 == 0 ? 2 : 5, saved);
        }
    }
}
