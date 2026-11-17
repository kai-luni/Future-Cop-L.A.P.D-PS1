// Categories: Graphics > HUD > Status Bar | Graphics > 2D Drawing
#include "game/types.h"

extern u8 gPlayerInvulnerable[] __asm__("D_8009FA8C");
extern volatile s32 gDiscShadeByte __asm__("D_8009FAA4");
extern s32 gSinglePlayerEntity[] __asm__("D_8009F9EC");

struct Unk6C080 {
    u8 pad00[0x84];
    s16 field84[3];
    s16 field8A[3];
    u8 field90[3];
    u8 pad93[0x2E];
    s8 fieldC1;
};

struct Arg6C080 {
    u8 pad00[0xAC];
    struct Unk6C080 *ptr;
};

extern void func_8006AAEC(u8 id);
extern void func_800491F8(s32 value, s32 x, s32 y, s32 color);
extern void func_8006BF44(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

/**
 * @brief Draws the three-slot status bar (icons plus counters).
 *
 * @param arg0 Owner record: sub-block at +0xAC whose arrays at +0x84, +0x8A and
 *        +0x90 carry the counts, and a flag array at +0xC1.
 * @return Nothing.
 *
 * @par Confirmed
 * A loop of three slots, stepping x and y by 0x29 each: func_8006AAEC(id) for the
 * icon, func_800491F8(count, x, 0xCE, color) for the number, and
 * func_8006BF44(i, highlight, y, 0xE0) for the sprite; the colour is 9 when bit
 * 0x40 of the frame counter D_8009FAA4 is set, else 0xC. A second pass draws the
 * 0xD8 line (with the alternative array +0x84 when the flag byte is set) and a
 * final func_8006BF44(-1, 0, 0x124, 0xE0) closes the bar.
 *
 * @par Likely role
 * The weapon/item bar: three slots at fixed 0x29 spacing, each with icon, count
 * and a highlighted frame.
 */
void func_8006C080(struct Arg6C080 *arg0)
{
    s32 i;
    struct Unk6C080 *d;
    register s32 flag __asm__("$19");
    s32 x;
    s32 y;
    register u8 *base __asm__("$22");

    i = 0;
    base = gPlayerInvulnerable;
    x = 0x33;
    d = arg0->ptr;
    y = 0x22;
    for (; i < 3; x += 0x29, i++, y += 0x29) {
        func_8006AAEC(d->field90[i]);
        flag = 0;
        if (*(u8 *)((u32)d->fieldC1 + (u32)base) == 0 && d->field8A[i] > 0) {
            flag = 1;
            func_800491F8(d->field8A[i], x, 0xCE, (gDiscShadeByte & 0x40) ? 0x9 : 0xC);
        }
        func_8006BF44(i, (*(u8 *)((u32)d->fieldC1 + (u32)base) != 0 || flag != 0) ? 1 : 0, y, 0xE0);
        func_800491F8((*(u8 *)((u32)d->fieldC1 + (u32)base) != 0) ? d->field8A[i] : d->field84[i], x, 0xD8, 8);
    }
    {
        s32 a0 = -1;
        s32 a2 = 0xC;
        s32 c;
        u8 *arr;
        __asm__("" : "=r"(a0) : "0"(a0));
        __asm__("" : "=r"(a2) : "0"(a2));
        c = d->fieldC1;
        __asm__("" : "=r"(c) : "0"(c));
        __asm__("" : "=r"(c) : "0"(c));
        arr = gPlayerInvulnerable;
        __asm__("" : "=r"(arr) : "0"(arr));
        func_8006BF44(a0, *(u8 *)((u32)c + (u32)arr), a2, 0xE0);
    }
    if (gSinglePlayerEntity[0] != 0) {
        func_8006BF44(-1, 0, 0x124, 0xE0);
    }
}
