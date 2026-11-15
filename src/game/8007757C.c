// Categories: Graphics > Debug Display | Platform > Debug
#include "game/types.h"

struct PacketState {
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    s16 values[4];
    u8 flags[4];
};

struct Text8 {
    char b[8];
};

extern struct Text8 D_8009F3B8;

extern void func_80091480(void);
extern void func_80091510(void *);
extern void func_80092910(void *);
extern void func_80092E10(void *, int, int, int);
extern void *func_80092140(void *, int, int, int, int);
extern void *func_800921F4(void *, int, int, int, int);
extern void func_800934BC(void *);
extern void func_800932F0(void *);
extern void func_80092BF4(int);
extern void func_80077278(int, int, unsigned int);

/**
 * @brief Draws a four-line debug text screen.
 *
 * @param a0..a3 The four numeric values printed at (0x20, 0x20), (0x20, 0x40),
 *        (0x20, 0x60) and (0x20, 0x80).
 * @return Nothing; the closing `__asm__(".word 0x000101CD")` is a `break`,
 *         i.e. the routine is a trap-style debug display that does not return in
 *         a useful way.
 *
 * @par Confirmed
 * The sequence is the standard stack of the 2D layer: func_80091480 /
 * func_80091510(0) / func_80092910(0) reset the display, func_80092E10(text, 0,
 * 0, 0) sets an 8-byte text block from D_8009F3B8, func_80092140 and
 * func_800921F4 build two 320x240 packet states, func_800934BC/starts and
 * func_800932F0 applies them, then four func_80077278(x, y, value) calls draw the
 * numbers.
 *
 * @par Likely role
 * A diagnostic overlay left in the retail build (func_800773D8 is its looping
 * twin, which ends in an endless redraw).
 */
void func_8007757C(int a0, int a1, int a2, int a3)
{
    u8 disp[0xB8];
    struct PacketState st[2];
    struct Text8 text;

    text = D_8009F3B8;

    func_80091480();
    func_80091510(0);
    func_80092910(0);
    func_80092E10(&text, 0, 0, 0);
    func_80092140(disp, 0, 0, 0x140, 0xF0);
    func_800921F4(st, 0, 0, 0x140, 0xF0);
    func_800934BC(st);
    func_800932F0(disp);
    func_80092BF4(1);

    func_80077278(0x20, 0x20, a0);
    func_80077278(0x20, 0x40, a1);
    func_80077278(0x20, 0x60, a2);
    func_80077278(0x20, 0x80, a3);

    __asm__ volatile(".word 0x000101CD");
}
