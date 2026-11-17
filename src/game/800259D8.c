// Categories: Game Engine > Screen Frame | Graphics > Display Setup
#include "game/types.h"

typedef struct {
    u8 unk00[0x10];
    s32 unk10;
} UnkFA94;

extern UnkFA94 *D_8009FA94;

extern s32 D_8009F9C4;
extern s32 gFrameId __asm__("D_8009F9FC");
extern s32 gScreenState __asm__("D_8009FA00");
extern s32 D_8009FA30;
extern volatile u8 gFrameBarrier __asm__("D_8009FA3C");
extern s32 gDiscShadeByte __asm__("D_8009FAA4");

extern u32 *gPrimPoolNext __asm__("D_8009F9F0");
extern u32 *gPrimPoolEnd __asm__("D_8009FA28");
extern volatile s32 gScreenOffsetValue __asm__("D_8009F9D4");
extern u8 *D_8009F9A0;
extern u8 *D_8009FA58[];

extern void func_8001BFD8(u8 value);
extern void func_80025370(void);
extern void func_80086134(void);
extern void func_80092660(u32 *arg0, s16 *arg1, s32 arg2, s32 arg3);
extern void func_8001B4A8(s32 arg0);
extern u8 func_8002385C(void);
extern void func_80025690(void);
extern void func_80024EC8(void);
extern void func_80047690(u8 enable);
extern void func_8006AB38(void *arg0);
extern void func_80056634(void);

/**
 * @brief Runs one front-end screen frame: prepare the screen, draw the
 *        background quad, submit it, then pump the module until it reports
 *        "done", and hand control back.
 *
 * @return 1 (the next screen state for the main loop), and the state block's
 *         unk10 is set to the same value.
 *
 * @par Confirmed flow
 * 1. D_8009FA30/D_8009FAA4 = 0, D_8009F9FC = 5, func_8001BFD8(0) arms the
 *    display, func_80025370() serves the pending loading screen and, in the
 *    "non-UI" mode (D_8009F9C4 == 0), func_80086134() prepares the image layer.
 * 2. Waits for the frame barrier D_8009FA3C to clear, then takes one 52-byte
 *    packet from the D_8009F9F0 pool.
 * 3. Builds a full-screen rectangle (0, 0x100 - offset, 0x140, 0xF0) and emits
 *    it with func_80092660(packet, rect, 0, colour) - a gouraud/flat quad that
 *    covers the screen (a backdrop or fade).
 * 4. func_8001B4A8(1) submits the frame.
 * 5. Idle loop: as long as D_8009FA00 still equals 4 and func_8002385C()
 *    returns non-zero, call func_80025690() (draw one effect quad) and
 *    func_80024EC8() (update the object list) - this is the screen's own
 *    frame pump.
 * 6. func_8001BFD8(1), func_80047690(1) release the display again, then the two
 *    entries of D_8009FA58 are passed to func_8006AB38 (freeing whatever the
 *    screen registered).
 * 7. If D_8009F9A0 is set func_80056634() runs a final step.
 * 8. Returns 1 with the state block's unk10 = 1.
 *
 * @par Globals
 * D_8009F9FC (frame id, 5), D_8009FAA4/D_8009FA30 (time base), D_8009FA3C
 * (barrier), D_8009F9F0/D_8009FA28 (packet pool), D_8009F9D4 (screen offset),
 * D_8009FA00 (screen state), D_8009F9C4 (mode), D_8009FA58 (two registered
 * pointers), D_8009F9A0.
 *
 * @par Callers
 * func_800287F4, as the first half of every screen.
 */
s32 func_800259D8(void)
{
    u32 *p;
    volatile s16 rect[4];
    s32 i;
    s32 one;
    register s32 color __asm__("$7");
    register s32 mode __asm__("$16");

    D_8009FA30 = 0;
    gDiscShadeByte = 0;
    gFrameId = 5;

    func_8001BFD8(0);
    func_80025370();

    if (D_8009F9C4 == 0) {
        func_80086134();
    }

    while (gFrameBarrier != 0) {
    }

    p = gPrimPoolNext;
    if (p < gPrimPoolEnd) {
        gPrimPoolNext = p + 0xD;
    }

    {
        register u32 masked __asm__("$2");
        register u32 half __asm__("$3");
        register u32 bits __asm__("$3");
        register volatile s16 *rp __asm__("$5");
        register s32 zero __asm__("$6");
        register u32 height __asm__("$2");
        mode = 4;
        __asm__("" : "+r"(mode));
        bits = 0xFF000000;
        rp = rect;
        __asm__("" : "+r"(rp));
        masked = p[0];
        color = gScreenOffsetValue;
        masked = masked & bits;
        __asm__("" : "+r"(masked) : : "$6");
        half = *(volatile u16 *)&gScreenOffsetValue;
        __asm__("" : "+r"(half) : : "$6");
        zero = 0;
        __asm__("" : "+r"(zero) : "r"(half));
        p[0] = masked;
        height = 0x100;
        __asm__("" : "+r"(height));
        rect[0] = 0;
        __asm__("" : "+r"(half) : : "memory");
        rect[1] = height - half;
        rect[2] = 0x140;
        *(s16 *)&rect[3] = 0xF0;
        func_80092660(p, rp, zero, color);
    }
    func_8001B4A8(1);

    while (1) {
        if (gScreenState == mode && func_8002385C() != 0) {
            break;
        }
        func_80025690();
        func_80024EC8();
    }

    func_8001BFD8(1);
    func_80047690(1);

    {
        register u8 **cursor __asm__("$16");

        i = 0;
        cursor = D_8009FA58;
        do {
            if (*cursor != 0) {
                func_8006AB38(*cursor);
            }
            i++;
            cursor++;
        } while (i < 2);
    }

    one = 1;
    if (D_8009F9A0 != 0) {
        func_80056634();
        one = 1;
    }

    gDiscShadeByte = 0;
    D_8009FA30 = 0;
    D_8009FA94->unk10 = one;
    return one;
}
